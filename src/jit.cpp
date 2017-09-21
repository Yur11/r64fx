#include "jit.hpp"
#include <sys/mman.h>
#include <limits>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace r64fx{

namespace{

unsigned char ModRM(unsigned char mod, unsigned char reg, unsigned char rm)
{
    reg &= b0111;
    rm  &= b0111;
    return (mod << 6) | (reg << 3) | rm;
}


unsigned char SIB(unsigned char scale, unsigned char index, unsigned char base)
{
    index &= b0111;
    base &= b0111;
    return (scale << 6) | (index << 3) | base;
}


unsigned char Rex(unsigned char bits)
{
    return (b0100 << 4) | bits;
}


unsigned char Rex(bool W, bool R, bool X, bool B)
{
    unsigned char bits = (W << 3) | (R << 2) | (X << 1) | B;
    return Rex(bits);
}


Imm32 Rip32(long addr, unsigned char* next_ip)
{
    long n_addr = (long) addr;
    long n_next_ip = (long) next_ip;

    long n_displacement = n_addr - n_next_ip;
#ifdef R64FX_DEBUG
    assert(n_displacement <= 0xFFFFFFFF);
#endif//R64FX_DEBUG

    return Imm32S((int) n_displacement);
}

}//namespace


#ifdef R64FX_JIT_DEBUG_STDOUT
const char* GPR64::names[] = {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

const char* Xmm::names[] = {
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
    "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
};

const char* CmpCode::names[] = {
    "eq", "lt", "le", "unord", "neq", "nlt", "nle", "ord"
};
#endif//R64FX_JIT_DEBUG_STDOUT


void Assembler::resize(unsigned long page_count)
{
#ifdef R64FX_DEBUG
    assert(page_count >= 0);
#endif//R64FX_DEBUG

    unsigned long new_size = page_count * memory_page_size();
    unsigned long old_size = m_size;

    if(new_size == 0)
    {
        if(old_size > 0)
        {
            mprotect(m_begin, old_size, PROT_READ | PROT_WRITE);
            free(m_begin);
            m_begin = m_end = nullptr;
            m_size = 0;
        }
    }
    else
    {
        auto new_buff = (unsigned char*) alloc_aligned(memory_page_size(), new_size);
        if(old_size > 0)
        {
            if(new_size > old_size)
            {
                memcpy(new_buff, m_begin, old_size);
            }
            mprotect(m_begin, old_size, PROT_READ | PROT_WRITE);
            free(m_begin);
        }
        auto end_offset = (unsigned long)(m_end - m_begin);
        m_begin = new_buff;
        m_end = m_begin + end_offset;
        m_size = new_size;
        mprotect(m_begin, new_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    }

    m_page_count = page_count;
}


void Assembler::write_bytes(unsigned char byte, int nbytes)
{
    ensureAvailable(nbytes);
    for(int i=0; i<nbytes; i++)
        m_end[i] = byte;
    m_end += nbytes;
}


void Assembler::write(unsigned char byte)
{
    ensureAvailable(1);
    m_end[0] = byte;
    m_end += 1;
}


void Assembler::write(unsigned char byte0, unsigned char byte1)
{
    ensureAvailable(2);
    m_end[0] = byte0;
    m_end[1] = byte1;
    m_end += 2;
}


void Assembler::write(unsigned char opcode, unsigned char r, GPR64 reg, Imm32 imm)
{
    ensureAvailable(7);
    m_end[0] = Rex(1, 0, 0, reg.prefix_bit());
    m_end[1] = opcode;
    m_end[2] = ModRM(b11, r, reg.code());
    for(int i=0; i<4; i++)
        m_end[i + 3] = imm.b[i];
    m_end += 7;
}


void Assembler::write(unsigned char opcode, GPR64 reg, Imm64 imm)
{
    ensureAvailable(10);
    m_end[0] = Rex(1, 0, 0, reg.prefix_bit());
    m_end[1] = opcode + (reg.code() & b0111);
    for(int i=0; i<8; i++)
        m_end[i + 2] = imm.b[i];
    m_end += 10;
}


void Assembler::write(unsigned char opcode, GPR64 dst, GPR64 src)
{
    ensureAvailable(3);
    m_end[0] = Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    m_end[1] = opcode;
    m_end[2] = ModRM(b11, dst.code(), src.code());
    m_end += 3;
}


void Assembler::write(unsigned char opcode, GPR64 reg, Mem64 mem)
{
    ensureAvailable(7);
    m_end[0] = Rex(1, reg.prefix_bit(), 0, 0);
    m_end[1] = opcode;
    m_end[2] = ModRM(b00, reg.code(), b101);
    auto rip = Rip32(mem.addr(), codeEnd() + 7);
    for(int i=0; i<4; i++) m_end[i + 3] = rip.b[i];
    m_end += 7;
}


void Assembler::write(unsigned char opcode, GPR64 reg, Base base, Disp8 disp)
{
    int nbytes = 4;

    bool use_disp = false;
    if(disp.byte != 0 || (base.reg.code() & b0111) == b101)
    {
        use_disp = true;
        nbytes++;
    }

    ensureAvailable(nbytes);

    m_end[0] = Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    m_end[1] = opcode;
    m_end[2] = ModRM(use_disp ? b01 : b00, reg.code(), b100);
    m_end[3] = SIB(b00, b100, base.reg.code());
    if(use_disp){ m_end[4] = disp.byte; }

    m_end += nbytes;
}


void Assembler::write(unsigned char opcode, Mem8 mem)
{
    ensureAvailable(5);
    m_end[0] = opcode;
    auto rip = Rip32(mem.addr(), ip() + 5);
    for(int i=0; i<4; i++)
        m_end[i + 1] = rip.b[i];
    m_end += 5;
}


void Assembler::write(unsigned char opcode1, unsigned char opcode2, Mem8 mem)
{
    ensureAvailable(6);
    m_end[0] = opcode1;
    m_end[1] = opcode2;
    auto rip = Rip32(mem.addr(), ip() + 6);
    for(int i=0; i<4; i++)
        m_end[i + 2] = rip.b[i];
    m_end += 6;
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

    ensureAvailable(nbytes);

    int r = 0;
    if(pre_rex_byte)
        { m_end[r++] = pre_rex_byte; }
    if(rex)
        { m_end[r++] = rex; }
    m_end[r++] = 0x0F;
    m_end[r++] = byte1;
    m_end[r++] = ModRM(b11, dst.code(), src.code());
    if(imm > 0) { m_end[r++] = (unsigned char)imm; }

    m_end += nbytes;
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

    ensureAvailable(nbytes);

    int r = 0;
    if(pre_rex_byte)
        { m_end[r++] = pre_rex_byte; }
    if(rex)
        { m_end[r++] = rex; }
    m_end[r++] = 0x0F;
    m_end[r++] = byte1;
    m_end[r++] = ModRM(b00, reg.code(), b101);
    auto rip = Rip32(mem.addr(), m_end + nbytes);
    for(int i=0; i<4; i++)
        m_end[r++] = rip.b[i];
    if(imm > 0) { m_end[r++] = (unsigned char)imm; }

    m_end += nbytes;
}


void Assembler::write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, Base base, Disp8 disp, int imm)
{
    int nbytes = 4;
    if(pre_rex_byte)
        nbytes++;

    unsigned char rex = 0;
    if(reg.prefix_bit() || base.reg.prefix_bit())
    {
        rex = Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
        nbytes++;
    }

    bool use_disp = false;
    if(disp.byte != 0 || (base.reg.code() & b0111) == b101)
    {
        use_disp = true;
        nbytes++;
    }

    if(imm > 0)
        nbytes++;
#ifdef R64FX_DEBUG
    assert(imm <= 0xFF);
#endif//R64FX_DEBUG

    ensureAvailable(nbytes);

    int r = 0;
    if(pre_rex_byte)
        { m_end[r++] = pre_rex_byte; }
    if(rex)
        { m_end[r++] = rex; }
    m_end[r++] = 0x0F;
    m_end[r++] = byte1;
    m_end[r++] = ModRM(use_disp ? b01 : b00, reg.code(), b100);
    m_end[r++] = SIB(b00, b100, base.reg.code());
    if(use_disp){ m_end[r++] = disp.byte; }
    if(imm > 0) { m_end[r++] = (unsigned char)imm; }

    m_end += nbytes;
}


void Assembler::write(unsigned char opcode, GPR64 reg)
{
    ensureAvailable(2);
    m_end[0] = Rex(1, 0, 0, reg.prefix_bit());
    m_end[1] = opcode + (reg.code() & b0111);
    m_end += 2;
}

}//namespace r64fx

