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


#ifdef R64FX_DEBUG_JIT_STDOUT
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
#endif//R64FX_DEBUG_JIT_STDOUT


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
    auto rip = Rip32(mem.addr, codeEnd() + 7);
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
    auto rip = Rip32(mem.addr, ip() + 5);
    for(int i=0; i<4; i++)
        m_end[i + 1] = rip.b[i];
    m_end += 5;
}


void Assembler::write(unsigned char opcode1, unsigned char opcode2, Mem8 mem)
{
    ensureAvailable(6);
    m_end[0] = opcode1;
    m_end[1] = opcode2;
    auto rip = Rip32(mem.addr, ip() + 6);
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


void Assembler::write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, long memaddr, int imm)
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
    auto rip = Rip32(memaddr, m_end + nbytes);
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


void Assembler::nop(int count)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " nop " << count << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x90, count);
}


void Assembler::ret()
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " ret\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0xC3);
}


void Assembler::rdtsc()
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " rdtsc\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x0F, 0x31);
}


void Assembler::rdpmc()
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " rdpmc\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x0F, 0x33);
}


void Assembler::mov(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", " << imm.s << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0xC7, 0, reg, imm);
}


void Assembler::mov(GPR64 reg, Imm64 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", " << imm.s << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0xB8, reg, imm);
}


void Assembler::mov(GPR64 dst, GPR64 src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x8B, dst, src);
}


void Assembler::mov(GPR64 reg, Mem64 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x8B, reg, mem);
}


void Assembler::mov(Mem64 mem, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x89, reg, mem);
}


void Assembler::mov(GPR64 reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", [" << base.reg.name() << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x8B, reg, base, disp);
}


void Assembler::mov(Base base, Disp8 disp, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    [" << base.reg.name() << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x89, reg, base, disp);
}


void Assembler::add(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << reg.name() << ", " << imm.s << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x81, 0, reg, imm);
}


void Assembler::add(GPR64 dst, GPR64 src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x03, dst, src);
}


void Assembler::add(GPR64 reg, Mem64 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x03, reg, mem);
}


void Assembler::add(Mem64 mem, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    [" << mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x01, reg, mem);
}


void Assembler::add(GPR64 reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << reg.name() << ", [" << base.reg.name() << "] + " << int(disp.byte) << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x03, reg, base, disp);
}


void Assembler::add(Base base, Disp8 disp, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    [" << base.reg.name() << "] + " << int(disp.byte) << ", " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x01, reg, base, disp);
}


void Assembler::sub(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    " << reg.name() << ", " << imm.s << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x81, 5, reg, imm);
}


void Assembler::sub(GPR64 dst, GPR64 src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x2B, dst, src);
}


void Assembler::sub(GPR64 reg, Mem64 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x2B, reg, mem);
}


void Assembler::sub(Mem64 mem, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x29, reg, mem);
}


void Assembler::sub(GPR64 reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    [" << base.reg.name() << "] + " << int(disp.byte) << ", " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x2B, reg, base, disp);
}


void Assembler::sub(Base base, Disp8 disp, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    [" << base.reg.name() << "] + " << int(disp.byte) << ", " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x29, reg, base, disp);
}


void Assembler::push(GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " push   " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(Rex(1, 0, 0, reg.prefix_bit()), (unsigned char)(0x50 + (reg.code() & b0111)));
}


void Assembler::pop(GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pop    " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(Rex(1, 0, 0, reg.prefix_bit()), (unsigned char)(0x58 + (reg.code() & b0111)));
}


void Assembler::cmp(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp   " << reg.name() << ", " << imm.s << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x81, 7, reg, imm);
}


void Assembler::jmp(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jnz   " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0xE9, mem);
}


void Assembler::jnz(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jnz    " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x0F, 0x85, mem);
}


void Assembler::jz(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jz    " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x0F, 0x84, mem);
}


void Assembler::je(Mem8 mem)
{
    jz(mem);
}


void Assembler::jne(Mem8 mem)
{
    jnz(mem);
}


void Assembler::jl(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jl   " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write(0x0F, 0x8C, mem);
}


void Assembler::movaps(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x28, dst, src);
}


void Assembler::movaps(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movaps " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x28, reg, mem.addr);
}


void Assembler::movaps(Mem128 mem, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movaps [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x29, reg, mem.addr);
}


void Assembler::movaps(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups" << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x28, reg, base, disp);
}


void Assembler::movaps(Base base, Disp8 disp, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x29, reg, base, disp);
}


void Assembler::movups(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x10, dst, src);
}


void Assembler::movups(Xmm reg, Mem32 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x10, reg, mem.addr);
}


void Assembler::movups(Mem32 mem, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x11, reg, mem.addr);
}


void Assembler::movups(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups" << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x10, reg, base, disp);
}


void Assembler::movups(Base base, Disp8 disp, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x11, reg, base, disp);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, second_opcode, dst, src);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, second_opcode, reg, mem.addr);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, second_opcode, reg, base, disp);
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, third_opcode, dst, src);
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm reg, Mem32 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, third_opcode, reg, mem.addr);
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, third_opcode, reg, base, disp);
}


void Assembler::cmpps(CmpCode kind, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ps    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0xC2, dst, src);
    write(kind.code());
}


/* Segfaults! */
void Assembler::cmpps(CmpCode kind, Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ps    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0xC2, reg, mem.addr);
    write(kind.code());
}


void Assembler::cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ps    " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int)disp.byte << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0xC2, reg, base, disp);
    write(kind.code());
}


void Assembler::cmpss(CmpCode kind, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ss    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, 0xC2, dst, src);
    write(kind.code());
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ss    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, 0xC2, reg, mem.addr);
    write(kind.code());
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ss    " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int)disp.byte << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, 0xC2, reg, base, disp);
    write(kind.code());
}


void Assembler::movss(Xmm reg, Mem32 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movss " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, 0x10, reg, mem.addr);
}


void Assembler::movss(Mem32 mem, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movss [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0xF3, 0x11, reg, mem.addr);
}


#ifdef R64FX_DEBUG_JIT_STDOUT
/* Debug stuff to print a single byte as 4 numbers, 2 bits each.*/
union ShufByte{
    unsigned char byte;
    struct{
        unsigned char b76:2;
        unsigned char b54:2;
        unsigned char b32:2;
        unsigned char b10:2;
    } pair;

    explicit ShufByte(unsigned char byte) : byte(byte) {}
};

std::ostream &operator<<(std::ostream &ost, ShufByte shufbyte)
{
    ost << (int)shufbyte.pair.b76 << ", " << (int)shufbyte.pair.b54 << ", " << (int)shufbyte.pair.b32 << ", " << (int)shufbyte.pair.b10;
    return ost;
}
#endif//R64FX_DEBUG_JIT_STDOUT

void Assembler::shufps(Xmm dst, Xmm src, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " shufps " << dst.name() << ", " << src.name() << ", ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0xC6, dst, src, imm);
}


void Assembler::shufps(Xmm reg, Mem128 mem, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " shufps " << reg.name() << ", [" << (void*)mem.addr << "], ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0xC6, reg, mem.addr, imm);
}


void Assembler::pshufd(Xmm dst, Xmm src, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pshufd " << dst.name() << ", " << src.name() << ", ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0x70, dst, src, imm);
}


void Assembler::pshufd(Xmm reg, Mem128 mem, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pshufd " << reg.name() << ", [" << (void*)mem.addr << "], ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0x70, reg, mem.addr, imm);
}


void Assembler::pshufd(Xmm reg, Base base, Disp8 disp, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pshufd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0x70, reg, base, disp, imm);
}


void Assembler::cvtps2dq(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtps2dq " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0x5B, dst, src);
}


void Assembler::cvtps2dq(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtps2dq " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0x5B, reg, mem.addr);
}


void Assembler::cvtps2dq(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtps2dq " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0x5B, reg, base, disp);
}


void Assembler::cvtdq2ps(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtdq2ps " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x5B, dst, src);
}


void Assembler::cvtdq2ps(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtdq2ps " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x5B, reg, mem.addr);
}


void Assembler::cvtdq2ps(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtdq2ps " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0, 0x5B, reg, base, disp);
}


void Assembler::paddd(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " paddd " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0xFE, dst, src);
}


void Assembler::paddd(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " paddd " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0xFE, reg, mem.addr);
}


void Assembler::paddd(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " paddd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0xFE, reg, base, disp);
}


void Assembler::psubd(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " psubd " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0xFA, dst, src);
}


void Assembler::psubd(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " psubd " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0xFA, reg, mem.addr);
}


void Assembler::psubd(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " psubd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT
    write0x0F(0x66, 0xFA, reg, base, disp);
}

}//namespace r64fx

