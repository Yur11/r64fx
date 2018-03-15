#include "jit.hpp"
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>

namespace r64fx{

namespace{

inline unsigned char WRXB(bool W, bool R, bool X, bool B)
    { return (int(W) << 3) | (int(R) << 2) | (int(X) << 1) | int(B); }

inline unsigned char Rex(bool W, bool R, bool X, bool B)
    { return 64 | WRXB(W, R, X, B); }

inline unsigned char ModRM(unsigned char mod, unsigned char reg, unsigned char rm)
    { return (mod << 6) | ((reg & 7) << 3) | (rm & 7); }

inline Imm32 Rip(long addr, unsigned char* next_ip)
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


inline void pack_operands(unsigned long &m, unsigned char r, Register rm, int nbytes)
{
    m |= ModRM(3, r, rm.lowerBits());
    m <<= 4; m |= WRXB(rm.rexW(), 0, 0, rm.prefixBit());
    m <<= 4; m |= nbytes;
}

}//namespace


Operands::Operands(unsigned char r)
{
    m |= ModRM(3, r, 0);
    m <<=8; m |= 1;
}


Operands::Operands(unsigned char r, Register rm)
{
    pack_operands(m, r, rm, 1);
}


Operands::Operands(unsigned char r, Register rm, Imm8 imm)
{
    m = imm.b; m <<= 8;
    pack_operands(m, r, rm, 2);
}


Operands::Operands(unsigned char r, Register rm, Imm32 imm)
{
    m = imm.n; m <<= 8;
    pack_operands(m, r, rm, 5);
}


Operands::Operands(Register r, Register rm, int imm)
{
    int nbytes = 1;
    if(!(imm & ~0xFF))
    {
        m = imm;
        m <<= 8;
        nbytes++;
    }
    m |= ModRM(3, r.lowerBits(), rm.lowerBits());
    m <<= 4; m |= WRXB(r.rexW() || rm.rexW(), r.prefixBit(), 0, rm.prefixBit());
    m <<= 4; m |= nbytes;
}


Operands::Operands(Register r, Mem8 mem, unsigned char* rip, int imm)
{
    int nbytes = 5;
    if(!(imm & ~0xFF))
    {
        m = imm;
        m <<= 32;
        nbytes++;
    }
    m |= Rip(mem.addr(), rip + int(r.rexW() || r.prefixBit())).n;
    m <<= 8; m |= ModRM(0, r.lowerBits(), 5);
    m <<= 4; m |= WRXB(r.rexW(), r.prefixBit(), 0, 0);
    m <<= 4; m |= nbytes;
}


Operands::Operands(Register r, SIBD sibd, int imm)
{
    int disp_byte_count = 0;
    if(sibd.hasDisplacement())
    {
        disp_byte_count = ((sibd.displacement() > 127 || sibd.displacement() < -128) ? 4 : 1);
    }

    unsigned char modrm = (r.lowerBits() << 3) | 4;
    unsigned char sib = 0;
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

    int nbytes = 2;
    if(!(imm & ~0xFF))
    {
        m |= imm;
        if(disp_byte_count == 4)
            m <<= 32;
        else
            m <<= 8;
        nbytes++;
    }
    if(disp_byte_count)
    {
        m |= sibd.displacement();
        m <<= 8;
        nbytes += disp_byte_count;
    }
    m |= sib;
    m <<= 8; m |= modrm;
    m <<= 4; m |= WRXB(r.rexW(), r.prefixBit(), sibd.index().prefixBit(), sibd.base().prefixBit());
    m <<= 4; m |= nbytes;
}


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


void AssemblerBuffers::write(unsigned char opcode, Register reg, Imm64 imm)
{
    auto p = growCode(10);
    p[0] = Rex(1, 0, 0, reg.prefixBit());
    p[1] = opcode + (reg.lowerBits());
    for(int i=0; i<8; i++)
        p[i + 2] = imm.b[i];
}


void AssemblerBuffers::write(unsigned char opcode, Register reg)
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
        imm = Rip((unsigned long)(codeBegin() + label.jmpAddr()), codeEnd());
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


void AssemblerBuffers::write(const Opcode &opcode, const Operands &operands)
{
    auto p = growCode(opcode.byteCount() + operands.totalByteCount());
    int r = 0;
    if(opcode.has66())
        p[r++] = 0x66;
    if(operands.hasRex())
        p[r++] = 64 | operands.wrxb();
    if(opcode.has0F())
        p[r++] = 0x0F;
    p[r++] = opcode.code();
    auto operand_bytes = operands.operandBytes();
    for(int i=0; i<operands.operandByteCount(); i++)
    {
        p[r++] = operand_bytes & 0xFF; operand_bytes >>= 8;
    }
}


void AssemblerBuffers::markLabel(JumpLabel &label)
{
    if(label.immAddr())
    {
        R64FX_DEBUG_ASSERT(label.jmpAddr() == 0);
        unsigned char* imm = codeBegin() + label.immAddr();
        Imm32 rip = Rip((unsigned long)codeEnd(), imm + 4);
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

