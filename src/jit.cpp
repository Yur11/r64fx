#include "jit.hpp"
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>

namespace r64fx{

namespace{

inline unsigned char Rex(bool W, bool R, bool X, bool B)
    { return b01000000 | (int(W) << 3) | (int(R) << 2) | (int(X) << 1) | int(B); }

inline unsigned char ModRM(unsigned char mod, unsigned char reg, unsigned char rm)
    { return (mod << 6) | ((reg & 7) << 3) | (rm & 7); }

inline Imm32 Rip32(long addr, unsigned char* next_ip)
{
    long displacement = long(addr) - long(next_ip);
#ifdef R64FX_DEBUG
    assert(displacement <= 0xFFFFFFFF);
#endif//R64FX_DEBUG
    return Imm32((int) displacement);
}

}//namespace


unsigned char SIBD::modrmByte(const Register &reg) const
{
    unsigned char mod = dispBytes();
    if(mod == 4) mod = 2;
    return ModRM(mod, reg.lowerBits(), 4);
}


unsigned char SIBD::sibByte() const
{
    unsigned char byte = scale() << 6;
    if(hasIndex())
        byte |= index().lowerBits() << 3;
    else
        byte |= 4 << 3;
    byte |= base().lowerBits();
    return byte;
}


int SIBD::dispBytes() const
{
    if(disp() != 0 || base().lowerBits() == 6)
    {
        if(disp() >= -128 && disp() <= 127)
            return 1;
        return 4;
    }
    return 0;
}


void Assembler::resize(unsigned long data_page_count, unsigned long code_page_count)
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


long Assembler::growData(int nbytes)
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


unsigned char* Assembler::growCode(int nbytes)
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


void Assembler::fill(unsigned char byte, int nbytes)
{
    auto p = growCode(nbytes);
    for(int i=0; i<nbytes; i++)
        p[i] = byte;
}


void Assembler::write(unsigned char byte)
{
    auto p = growCode(1);
    p[0] = byte;
}


void Assembler::write(unsigned char byte0, unsigned char byte1)
{
    auto p = growCode(2);
    p[0] = byte0;
    p[1] = byte1;
}


void Assembler::write(unsigned char opcode, unsigned char r, GPR64 reg, Imm32 imm)
{
    auto p = growCode(7);
    p[0] = Rex(1, 0, 0, reg.prefix_bit());
    p[1] = opcode;
    p[2] = ModRM(b11, r, reg.lowerBits());
    for(int i=0; i<4; i++)
        p[i + 3] = imm.b[i];
}


void Assembler::write(unsigned char opcode, GPR64 reg, Imm64 imm)
{
    auto p = growCode(10);
    p[0] = Rex(1, 0, 0, reg.prefix_bit());
    p[1] = opcode + (reg.lowerBits());
    for(int i=0; i<8; i++)
        p[i + 2] = imm.b[i];
}


void Assembler::write(unsigned char opcode, GPR64 dst, GPR64 src)
{
    auto p = growCode(3);
    p[0] = Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    p[1] = opcode;
    p[2] = ModRM(b11, dst.lowerBits(), src.lowerBits());
}


void Assembler::write(unsigned char opcode, GPR64 reg, Mem64 mem)
{
    auto p = growCode(7);
    p[0] = Rex(1, reg.prefix_bit(), 0, 0);
    p[1] = opcode;
    p[2] = ModRM(b00, reg.lowerBits(), b101);
    auto rip = Rip32(mem.addr(), codeEnd());
    for(int i=0; i<4; i++) p[i + 3] = rip.b[i];
}


void Assembler::write(unsigned char opcode, GPR64 reg, SIBD sibd)
{
    int nbytes = 4 + sibd.dispBytes();
    auto p = growCode(nbytes);
    p[0] = Rex(1, reg.prefix_bit(), sibd.index().prefix_bit(), sibd.base().prefix_bit());
    p[1] = opcode;
    p[2] = sibd.modrmByte(reg);
    p[3] = sibd.sibByte();
    Imm32 disp(sibd.disp());
    for(int i=0; i<sibd.dispBytes(); i++) { p[i + 4] = disp.b[i]; }
}


void Assembler::write(unsigned char opcode, Mem8 mem)
{
    auto p = growCode(5);
    p[0] = opcode;
    auto rip = Rip32(mem.addr(), codeEnd());
    for(int i=0; i<4; i++)
        p[i + 1] = rip.b[i];
    p += 5;
}


void Assembler::write(unsigned char opcode1, unsigned char opcode2, Mem8 mem)
{
    auto p = growCode(6);
    p[0] = opcode1;
    p[1] = opcode2;
    auto rip = Rip32(mem.addr(), codeEnd());
    for(int i=0; i<4; i++)
        p[i + 2] = rip.b[i];
}


void Assembler::write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm dst, Xmm src, int imm)
{
    int nbytes = 3;
    if(pre_rex_byte)
        nbytes++;

    unsigned char rex = 0;
    if(dst.prefix_bit() || src.prefix_bit())
    {
        rex = Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
        nbytes++;
    }

    if(imm > 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    auto p = growCode(nbytes);

    int r = 0;
    if(pre_rex_byte)
        { p[r++] = pre_rex_byte; }
    if(rex)
        { p[r++] = rex; }
    p[r++] = 0x0F;
    p[r++] = byte1;
    p[r++] = ModRM(b11, dst.lowerBits(), src.lowerBits());
    if(imm > 0) { p[r++] = (unsigned char)imm; }
}


void Assembler::write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, Mem8 mem, int imm)
{
    int nbytes = 7;
    if(pre_rex_byte)
        nbytes++;

    unsigned char rex = 0;
    if(reg.prefix_bit())
    {
        rex = Rex(0, reg.prefix_bit(), 0, 0);
        nbytes++;
    }

    if(imm > 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    auto p = growCode(nbytes);

    int r = 0;
    if(pre_rex_byte)
        { p[r++] = pre_rex_byte; }
    if(rex)
        { p[r++] = rex; }
    p[r++] = 0x0F;
    p[r++] = byte1;
    p[r++] = ModRM(b00, reg.lowerBits(), b101);
    auto rip = Rip32(mem.addr(), codeEnd());
    for(int i=0; i<4; i++)
        p[r++] = rip.b[i];
    if(imm > 0) { p[r++] = (unsigned char)imm; }
}


void Assembler::write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, SIBD sibd, int imm)
{
    int nbytes = 4;
    if(pre_rex_byte)
        nbytes++;

    unsigned char rex = 0;
    if(reg.prefix_bit() || sibd.index().prefix_bit() || sibd.base().prefix_bit())
    {
        rex = Rex(0, reg.prefix_bit(), sibd.index().prefix_bit(), sibd.base().prefix_bit());
        nbytes++;
    }

    nbytes += sibd.dispBytes();

    if(imm > 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    auto p = growCode(nbytes);

    int r = 0;
    if(pre_rex_byte)
        { p[r++] = pre_rex_byte; }
    if(rex)
        { p[r++] = rex; }
    p[r++] = 0x0F;
    p[r++] = byte1;
    p[r++] = sibd.modrmByte(reg);
    p[r++] = sibd.sibByte();
    Imm32 disp(sibd.disp());
    for(int i=0; i<sibd.dispBytes(); i++) { p[r++] = disp.b[i]; }
    if(imm > 0) { p[r++] = (unsigned char)imm; }
}


void Assembler::write(unsigned char opcode, GPR64 reg)
{
    auto p = growCode(2);
    p[0] = Rex(1, 0, 0, reg.prefix_bit());
    p[1] = opcode + (reg.lowerBits());
}


void Assembler::write(unsigned char opcode1, unsigned char opcode2, JumpLabel &label)
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


void Assembler::mark(JumpLabel &label)
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

