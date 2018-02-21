#include "jit.hpp"
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>

namespace r64fx{

namespace{

inline unsigned char Rex(bool W, bool R, bool X, bool B)
    { return 64 | (int(W) << 3) | (int(R) << 2) | (int(X) << 1) | int(B); }

inline unsigned char ModRM(unsigned char mod, unsigned char reg, unsigned char rm)
    { return (mod << 6) | ((reg & 7) << 3) | (rm & 7); }

inline Imm32 Rip32(long addr, unsigned char* next_ip)
{
    long offset = long(addr) - long(next_ip);
#ifdef R64FX_DEBUG
    assert(offset <= 0xFFFFFFFF);
#endif//R64FX_DEBUG
    return Imm32(int(offset));
}

struct SibEncoding{
    unsigned char rex              = 0;
    unsigned char modrm            = 0;
    unsigned char sib              = 0;
    unsigned char disp_byte_count  = 0;

    SibEncoding(Register reg, SIBD sibd)
    {
        rex = Rex(
            reg.rexW(),
            reg.prefixBit(),
            sibd.index().prefixBit(),
            sibd.base().prefixBit()
        );

        if(sibd.hasDisplacement())
        {
            disp_byte_count = ((sibd.displacement() > 127 || sibd.displacement() < -128) ? 4 : 1);
        }

        modrm = (reg.lowerBits() << 3) | 4;
        if(sibd.hasBase())
        {
            if(disp_byte_count == 0 && sibd.base().lowerBits() == 5)
            {
                disp_byte_count = 1;
            }

            if(disp_byte_count == 1)
            {
                modrm |= (1 << 6);
            }
            else if(disp_byte_count == 4)
            {
                modrm |= (2 << 6);
            }

            sib = ((sibd.hasIndex() ? sibd.index().lowerBits() : 4) << 3) | sibd.base().lowerBits();
        }
        else
        {
            disp_byte_count = 4;
            sib = ((sibd.hasIndex() ? sibd.index().lowerBits() : 4) << 3) | 5;
        }
        sib |= (sibd.scaleBits() << 6);
    }
};

}//namespace


void AssemblerBuffers::resize(unsigned long data_page_count, unsigned long code_page_count)
{
    unsigned long new_page_count = data_page_count + code_page_count;
    unsigned long old_page_count = dataPageCount() + codePageCount();

    if(new_page_count == 0)
    {
        if(old_page_count > 0)
        {
            mprotect(m_buffer, old_page_count * memory_page_size(), PROT_READ | PROT_WRITE);
            free(m_buffer);
            m_buffer = m_data_begin = m_code_begin = m_code_end = m_buffer_end = nullptr;
        }
    }
    else
    {
        auto new_size        = new_page_count * memory_page_size();
        auto new_buff        = (unsigned char*) alloc_aligned(memory_page_size(), new_size);
        auto new_code_begin  = new_buff + data_page_count * memory_page_size();
        auto new_code_end    = new_code_begin + codeBytesUsed();
        auto new_data_begin  = new_code_begin - dataBytesUsed();

        if(old_page_count > 0)
        {
            if(data_page_count >= dataPageCount())
            {
                memcpy(new_data_begin, dataBegin(), dataBytesUsed());
            }

            if(code_page_count >= codePageCount())
            {
                memcpy(new_code_begin, codeBegin(), codeBytesUsed());
            }

            mprotect(m_buffer, old_page_count * memory_page_size(), PROT_READ | PROT_WRITE);
            free(m_buffer);
        }

        m_buffer      = new_buff;
        m_data_begin  = new_data_begin;
        m_code_begin  = new_code_begin;
        m_code_end    = new_code_end;
        m_buffer_end  = m_buffer + new_size;

        if(data_page_count)
            mprotect(m_buffer,     dataBufferSize(),  PROT_READ | PROT_WRITE);
        if(code_page_count)
            mprotect(codeBegin(),  codeBufferSize(),  PROT_READ | PROT_WRITE | PROT_EXEC);

    }
}


long AssemblerBuffers::growData(int nbytes)
{
    long bytes_available = dataBytesAvailable();
    long new_page_count = 0;
    while(bytes_available < nbytes)
    {
        bytes_available += memory_page_size();
        new_page_count++;
    }
    if(new_page_count > 0)
    {
        resize(dataPageCount() + new_page_count, codePageCount());
    }
    m_data_begin -= nbytes;
    return m_code_begin - m_data_begin;
}


unsigned char* AssemblerBuffers::growCode(int nbytes)
{
    int new_bytes_needed = nbytes - codeBytesAvailable();
    if(new_bytes_needed > 0)
    {
        int new_pages = new_bytes_needed / memory_page_size() + 1;
        resize(dataPageCount(), codePageCount() + new_pages);
    }
    auto result = m_code_end;
    m_code_end += nbytes;
    return result;
}


void AssemblerBuffers::fill(unsigned char byte, int nbytes)
{
    auto p = growCode(nbytes);
    for(int i=0; i<nbytes; i++)
        p[i] = byte;
}


void AssemblerBuffers::write(unsigned char byte)
{
    auto p = growCode(1);
    p[0] = byte;
}


void AssemblerBuffers::write(unsigned char byte0, unsigned char byte1)
{
    auto p = growCode(2);
    p[0] = byte0;
    p[1] = byte1;
}


void AssemblerBuffers::write(unsigned char opcode, unsigned char r, GPR reg)
{
    unsigned char rex = 0;
    if(reg.rex())
        rex = Rex(reg.rexW(), 0, 0, reg.prefixBit());
    auto p = growCode(rex ? 3 : 2);
    int n = 0;
    if(rex) p[n++] = rex;
    p[n++] = opcode;
    p[n++] = ModRM(3, r, reg.lowerBits());
}


void AssemblerBuffers::write(unsigned char opcode, unsigned char r, GPR reg, Imm8 imm)
{
    unsigned char rex = 0;
    if(reg.rex())
        rex = Rex(reg.rexW(), 0, 0, reg.prefixBit());
    auto p = growCode(rex ? 4 : 3);
    int n = 0;
    if(rex) p[n++] = rex;
    p[n++] = opcode;
    p[n++] = ModRM(3, r, reg.lowerBits());
    p[n++] = imm.b;
}


void AssemblerBuffers::write(unsigned char opcode, unsigned char r, GPR reg, Imm32 imm)
{
    unsigned char rex = 0;
    if(reg.rex())
        rex = Rex(reg.rexW(), 0, 0, reg.prefixBit());
    auto p = growCode(rex ? 7 : 6);
    int n = 0;
    if(rex) p[n++] = rex;
    p[n++] = opcode;
    p[n++] = ModRM(3, r, reg.lowerBits());
    for(int i=0; i<4; i++)
        p[n++] = imm.b[i];
}


void AssemblerBuffers::write(unsigned char opcode, GPR64 reg, Imm64 imm)
{
    auto p = growCode(10);
    p[0] = Rex(1, 0, 0, reg.prefixBit());
    p[1] = opcode + (reg.lowerBits());
    for(int i=0; i<8; i++)
        p[i + 2] = imm.b[i];
}


void AssemblerBuffers::write(unsigned char opcode, GPR dst, GPR64 src)
{
    auto p = growCode(3);
    p[0] = Rex((dst.rexW() || src.rexW()), dst.prefixBit(), 0, src.prefixBit());
    p[1] = opcode;
    p[2] = ModRM(3, dst.lowerBits(), src.lowerBits());
}


void AssemblerBuffers::write(unsigned char opcode, GPR reg, Mem32 mem)
{
    unsigned char rex = 0;
    if(reg.rex())
        rex = Rex(reg.rexW(), reg.prefixBit(), 0, 0);
    int nbytes = (rex ? 7 : 6);
    auto rip = Rip32(mem.addr(), codeEnd() + nbytes);
    auto p = growCode(nbytes);
    int r = 0;
    if(rex) p[r++] = rex;
    p[r++] = opcode;
    p[r++] = ModRM(0, reg.lowerBits(), 5);
    for(int i=0; i<4; i++) p[r++] = rip.b[i];
}


void AssemblerBuffers::write(unsigned char opcode, GPR reg, SIBD sibd)
{
    SibEncoding e(reg, sibd);

    int nbytes = 4 + e.disp_byte_count;
    auto p = growCode(nbytes);
    p[0] = e.rex;
    p[1] = opcode;
    p[2] = e.modrm;
    p[3] = e.sib;
    Imm32 disp(sibd.displacement());
    for(int i=0; i<e.disp_byte_count; i++) { p[i + 4] = disp.b[i]; }
}


void AssemblerBuffers::write0x0F(unsigned char prefix, unsigned opcode, Register reg, Register rm, int imm)
{
    int nbytes = 3;
    if(prefix)
        nbytes++;

    unsigned char rex = 0;
    if(reg.rex() || rm.rex())
    {
        rex = Rex(reg.rexW() || rm.rexW(), reg.prefixBit(), 0, rm.prefixBit());
        nbytes++;
    }

    if(imm >= 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    auto p = growCode(nbytes);

    int r = 0;
    if(prefix)
        p[r++] = prefix;
    if(rex)
        p[r++] = rex;
    p[r++] = 0x0F;
    p[r++] = opcode;
    p[r++] = ModRM(3, reg.lowerBits(), rm.lowerBits());
    if(imm >= 0) { p[r++] = (unsigned char)imm; }
}


void AssemblerBuffers::write0x0F(unsigned char prefix, unsigned opcode, Xmm reg, Mem8 mem, int imm)
{
    int nbytes = 7;
    if(prefix)
        nbytes++;

    unsigned char rex = 0;
    if(reg.rex())
    {
        rex = Rex(reg.rexW(), reg.prefixBit(), 0, 0);
        nbytes++;
    }

    if(imm >= 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    auto rip = Rip32(mem.addr(), codeEnd() + nbytes);
    auto p = growCode(nbytes);

    int r = 0;
    if(prefix)
        p[r++] = prefix;
    if(rex)
        p[r++] = rex;
    p[r++] = 0x0F;
    p[r++] = opcode;
    p[r++] = ModRM(0, reg.lowerBits(), 5);
    for(int i=0; i<4; i++)
        p[r++] = rip.b[i];
    if(imm >= 0) { p[r++] = (unsigned char)imm; }
}


void AssemblerBuffers::write0x0F(unsigned char prefix, unsigned opcode, Xmm reg, SIBD sibd, int imm)
{
    int nbytes = 4;
    if(prefix)
        nbytes++;

    SibEncoding e(reg, sibd);

    unsigned char rex = 0;
    if((e.rex & 0xF) != 0)
        { rex = e.rex; nbytes++; }
    nbytes += e.disp_byte_count;

    if(imm >= 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    auto p = growCode(nbytes);

    int r = 0;
    if(prefix)
        p[r++] = prefix;
    if(rex)
        p[r++] = rex;
    p[r++] = 0x0F;
    p[r++] = opcode;
    p[r++] = e.modrm;
    p[r++] = e.sib;
    Imm32 disp(sibd.displacement());
    for(int i=0; i<e.disp_byte_count; i++) { p[r++] = disp.b[i]; }
    if(imm >= 0) { p[r++] = (unsigned char)imm; }
}


void AssemblerBuffers::write(unsigned char opcode, GPR64 reg)
{
    auto p = growCode(2);
    p[0] = Rex(1, 0, 0, reg.prefixBit());
    p[1] = opcode + (reg.lowerBits());
}


void AssemblerBuffers::write(unsigned char opcode1, unsigned char opcode2, JumpLabel &label)
{
    int nbytes = 5;
    if(opcode1)
        nbytes++;

    auto p = growCode(nbytes);

    int r = 0;
    if(opcode1)
        p[r++] = opcode1;
    p[r++] = opcode2;

    Imm32 imm = Imm32(0);
    if(label.jmpAddr())
    {
        imm = Rip32((unsigned long)(codeBegin() + label.jmpAddr()), codeEnd());
    }
    else
    {
        long offset = p - codeBegin() + r;
#ifdef R64FX_DEBUG
        assert(offset <= 0x7FFFFFFF || offset >= -0x7FFFFFFF);
        assert(label.immAddr() == 0);
#endif//R64FX_DEBUG
        label.setImmAddr(offset);
    }

    for(int i=0; i<4; i++)
    {
        p[r++] = imm.b[i];
    }
}


void AssemblerBuffers::markLabel(JumpLabel &label)
{
    if(label.immAddr())
    {
        R64FX_DEBUG_ASSERT(label.jmpAddr() == 0);
        unsigned char* imm = codeBegin() + label.immAddr();
        Imm32 rip = Rip32((unsigned long)codeEnd(), imm + 4);
        for(int i=0; i<4; i++)
        {
            imm[i] = rip.b[i];
        }
    }

    long offset = codeBytesUsed();
    R64FX_DEBUG_ASSERT(offset <= 0x7FFFFFFF || offset >= -0x7FFFFFFF);
    label.setJmpAddr(int(offset));
}

}//namespace r64fx

