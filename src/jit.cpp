#include "jit.hpp"
#include <sys/mman.h>
#include <limits>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace r64fx{


CodeBuffer::CodeBuffer(int page_count)
{
    resize(page_count);
}


CodeBuffer::~CodeBuffer()
{
    resize(0);
}


void CodeBuffer::resize(int page_count)
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


CodeBuffer &CodeBuffer::operator<<(unsigned char byte)
{
    if(bytesAvailable() == 0)
        resize(m_page_count + 1);
    *m_end = byte;
    m_end += 1;
    return *this;
}


CodeBuffer &CodeBuffer::operator<<(Imm16 imm)
{
    if(bytesAvailable() == 1)
        resize(m_page_count + 1);
    for(int i=0; i<2; i++)
    {
        *m_end = imm.b[i];
        m_end += 1;
    }
    return *this;
}


CodeBuffer &CodeBuffer::operator<<(Imm32 imm)
{
    if(bytesAvailable() == 3)
        resize(m_page_count + 1);
    for(int i=0; i<4; i++)
    {
        *m_end = imm.b[i];
        m_end += 1;
    }
    return *this;
}


CodeBuffer &CodeBuffer::operator<<(Imm64 imm)
{
    if(bytesAvailable() == 7)
        resize(m_page_count + 1);
    for(int i=0; i<8; i++)
    {
        *m_end = imm.b[i];
        m_end += 1;
    }
    return *this;
}


#ifdef R64FX_DEBUG_JIT_STDOUT
const char* GPR64::names[] = {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

const char* GPR32::names[] = {
    "eax", "ecx", "edx", "ebx", "esp", "ebp", "edi", "esi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
};

const char* GPR16::names[] = {
    "ax", "cx", "dx", "bx", "sp", "bp", "di", "si"
};

const char* GPR8::names[] = {
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"
};

const char* Xmm::names[] = {
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
    "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
};
#endif//R64FX_DEBUG_JIT_STDOUT


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


struct Rip32{
    int displacement;

    Rip32(long addr, unsigned char* next_ip)
    {
        long n_addr = (long) addr;
        long n_next_ip = (long) next_ip;

        long n_displacement = n_addr - n_next_ip;
#ifdef R64FX_DEBUG
        assert(n_displacement <= 0xFFFFFFFF);
#endif//R64FX_DEBUG

        displacement = (int) n_displacement;
    }
};


CodeBuffer &operator<<(CodeBuffer &buff, Rip32 rip)
{
    return buff << Imm32S(rip.displacement);
}


void encode_modrm_sib_base_and_disp8(CodeBuffer* bytes, Register &reg, Base &base, Disp8 disp)
{
    *bytes << ModRM(b01, reg.code(), b100);
    *bytes << SIB(b00, b100, base.reg.code());
    *bytes << Imm8U(disp.byte);
}


void encode_modrm_and_sib_base(CodeBuffer* bytes, Register &reg, Base &base)
{
    if((base.reg.code() & b0111) == b101)
    {
        encode_modrm_sib_base_and_disp8(bytes, reg, base, Disp8(0));
    }
    else
    {
        *bytes << ModRM(b00, reg.code(), b100);
        *bytes << SIB(b00, b100, base.reg.code());
    }
}


unsigned char Shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3)
{
    return (s3 << 6) + (s2 << 4) + (s1 << 2) + s0;
}


#ifdef R64FX_DEBUG_JIT_STDOUT
const char* CmpCode::names[] = {
    "eq", "lt", "le", "unord", "neq", "nlt", "nle", "ord"
};
#endif//R64FX_DEBUG_JIT_STDOUT


void Assembler::mov(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", " << (int)imm << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, 0, 0, reg.prefix_bit());
    *m_bytes << 0xC7;
    *m_bytes << ModRM(b11, 0, reg.code());
    *m_bytes << imm;
}


void Assembler::mov(GPR64 reg, Imm64 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", " << (long) imm << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, 0, 0, reg.prefix_bit());
    *m_bytes << (0xB8 + (reg.code() & b0111)) << imm;
}


void Assembler::mov(GPR64 dst, GPR64 src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x8B;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::mov(GPR64 reg, Mem64 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x8B;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::mov(Mem64 mem, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x89;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::mov(GPR64 reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    " << reg.name() << ", [" << base.reg.name() << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x8B;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::mov(Base base, Disp8 disp, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " mov    [" << base.reg.name() << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x89;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::add(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << reg.name() << ", " << (int)imm << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, 0, 0, reg.prefix_bit());
    *m_bytes << 0x81;
    *m_bytes << ModRM(b11, 0, reg.code());
    *m_bytes << imm;
}


void Assembler::add(GPR64 dst, GPR64 src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x03;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::add(GPR64 reg, Mem64 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x03;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::add(Mem64 mem, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    [" << mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x01;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::add(GPR64 reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    " << reg.name() << ", [" << base.reg.name() << "] + " << int(disp.byte) << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x03;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::add(Base base, Disp8 disp, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " add    [" << base.reg.name() << "] + " << int(disp.byte) << ", " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x01;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::sub(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    " << reg.name() << ", " << (int)imm << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, 0, 0, reg.prefix_bit());
    *m_bytes << 0x81;
    *m_bytes << ModRM(b11, 5, reg.code());
    *m_bytes << imm;
}


void Assembler::sub(GPR64 dst, GPR64 src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x2B;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sub(GPR64 reg, Mem64 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x2B;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::sub(Mem64 mem, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x29;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::sub(GPR64 reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    [" << base.reg.name() << "] + " << int(disp.byte) << ", " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x2B;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::sub(Base base, Disp8 disp, GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " sub    [" << base.reg.name() << "] + " << int(disp.byte) << ", " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x29;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::push(GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " push  " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, 0, 0, reg.prefix_bit());
    *m_bytes << (0x50 + (reg.code() & b0111));
}


void Assembler::pop(GPR64 reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pop   " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, 0, 0, reg.prefix_bit());
    *m_bytes << (0x58 + (reg.code() & b0111));
}


void Assembler::cmp(GPR64 reg, Imm32 imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp   " << reg.name() << ", " << (int)imm << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << Rex(1, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x81;
    *m_bytes << ModRM(b11, 7, reg.code());
    *m_bytes << imm;
}


void Assembler::jmp(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jnz   " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xE9;
    *m_bytes << Rip32(mem.addr, ip() + 4);
}


void Assembler::jnz(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jnz    " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x0F << 0x85;
    *m_bytes << Rip32(mem.addr, ip() + 4);
}


void Assembler::jz(Mem8 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " jz    " << (void*)mem.addr << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x0F << 0x84;
    *m_bytes << Rip32(mem.addr, ip() + 4);
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

    *m_bytes << 0x0F << 0x8C;
    *m_bytes << Rip32(mem.addr, ip() + 4);
}


void Assembler::movaps(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0x28;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::movaps(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movaps " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x28;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::movaps(Mem128 mem, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movaps [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x29;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::movaps(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups" << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x28;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::movaps(Base base, Disp8 disp, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x29;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::movups(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0x10;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::movups(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x10;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::movups(Mem128 mem, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x11;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::movups(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups" << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x10;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::movups(Base base, Disp8 disp, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movups [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x11;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << second_opcode;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << second_opcode;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << second_opcode;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


#ifdef R64FX_DEBUG_JIT_STDOUT
#define DUMP_IP_AND_NAME(name) std::cout << (void*)ip() << " " << #name;
#else
#define DUMP_IP_AND_NAME(name)
#endif//R64FX_DEBUG_JIT_STDOUT


#define ENCODE_SSE_PS_INSTRUCTION(name, second_opcode)\
void Assembler::name(Xmm dst, Xmm src)\
{\
    DUMP_IP_AND_NAME(name)\
    sse_ps_instruction(second_opcode, dst, src);\
}\
\
\
void Assembler::name(Xmm reg, Mem128 mem)\
{\
    DUMP_IP_AND_NAME(name)\
    sse_ps_instruction(second_opcode, reg, mem);\
}\
\
\
void Assembler::name(Xmm reg, Base base, Disp8 disp)\
{\
    DUMP_IP_AND_NAME(name)\
    sse_ps_instruction(second_opcode, reg, base, disp);\
}\



void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << third_opcode;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm reg, Mem32 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << third_opcode;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << "  " << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(reg.prefix_bit() || base.reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << third_opcode;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


#define ENCODE_SSE_SS_INSTRUCTION(name, third_opcode)\
void Assembler::name(Xmm dst, Xmm src)\
{\
    DUMP_IP_AND_NAME(name)\
    sse_ss_instruction(third_opcode, dst, src);\
}\
\
\
void Assembler::name(Xmm reg, Mem32 mem)\
{\
    DUMP_IP_AND_NAME(name)\
    sse_ss_instruction(third_opcode, reg, mem);\
}\
\
\
void Assembler::name(Xmm reg, Base base, Disp8 disp)\
{\
    DUMP_IP_AND_NAME(name)\
    sse_ss_instruction(third_opcode, reg, base, disp);\
}\


#define ENCODE_SSE_INSTRUCTION(name, opcode)\
    ENCODE_SSE_PS_INSTRUCTION(name##ps, opcode)\
    ENCODE_SSE_SS_INSTRUCTION(name##ss, opcode)


ENCODE_SSE_INSTRUCTION(add,   0x58)
ENCODE_SSE_INSTRUCTION(sub,   0x5C)
ENCODE_SSE_INSTRUCTION(mul,   0x59)
ENCODE_SSE_INSTRUCTION(div,   0x5E)
ENCODE_SSE_INSTRUCTION(rcp,   0x53)
ENCODE_SSE_INSTRUCTION(sqrt,  0x51)
ENCODE_SSE_INSTRUCTION(rsqrt, 0x52)
ENCODE_SSE_INSTRUCTION(max,   0x5F)
ENCODE_SSE_INSTRUCTION(min,   0x5D)
ENCODE_SSE_PS_INSTRUCTION(andps,   0x54)
ENCODE_SSE_PS_INSTRUCTION(andnps,  0x55)
ENCODE_SSE_PS_INSTRUCTION(orps,    0x56)
ENCODE_SSE_PS_INSTRUCTION(xorps,   0x57)


void Assembler::cmpps(CmpCode kind, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ps    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0xC2;
    *m_bytes << ModRM(b11, dst.code(), src.code());
    *m_bytes << kind.code();
}


/* Segfaults! */
void Assembler::cmpps(CmpCode kind, Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ps    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0xC2;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 5);
    *m_bytes << kind.code();
}


void Assembler::cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ps    " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int)disp.byte << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0xC2;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
    *m_bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ss    " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0xC2;
    *m_bytes << ModRM(b11, dst.code(), src.code());
    *m_bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ss    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0xC2;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
    *m_bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cmp" << kind.name() << "ss    " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int)disp.byte << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(reg.prefix_bit() || base.reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0xC2;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
    *m_bytes << kind.code();
}


void Assembler::movss(Xmm reg, Mem32 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movss " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x10;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::movss(Mem32 mem, Xmm reg)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " movss [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0xF3;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x11;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
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

    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0xC6;
    *m_bytes << ModRM(b11, dst.code(), src.code());
    *m_bytes << imm;
}


void Assembler::shufps(Xmm reg, Mem128 mem, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " shufps " << reg.name() << ", [" << (void*)mem.addr << "], ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0xC6;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 5);
    *m_bytes << imm;
}


void Assembler::pshufd(Xmm dst, Xmm src, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pshufd " << dst.name() << ", " << src.name() << ", ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit()) *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0x70;
    *m_bytes << ModRM(b11, dst.code(), src.code());
    *m_bytes << imm;
}


void Assembler::pshufd(Xmm reg, Mem128 mem, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pshufd " << reg.name() << ", [" << (void*)mem.addr << "], ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x70;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 5);
    *m_bytes << imm;
}


void Assembler::pshufd(Xmm reg, Base base, Disp8 disp, unsigned char imm)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " pshufd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
    std::cout << "(" << ShufByte(imm) <<  ")\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x70;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
    *m_bytes << imm;
}


void Assembler::cvtps2dq(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtps2dq " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit())
        *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0x5B;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::cvtps2dq(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtps2dq " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x5B;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::cvtps2dq(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtps2dq " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x5B;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::cvtdq2ps(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtdq2ps " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(dst.prefix_bit() || src.prefix_bit())
        *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0x5B;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::cvtdq2ps(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtdq2ps " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0x5B;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::cvtdq2ps(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " cvtdq2ps " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0x5B;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::paddd(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " paddd " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit())
        *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0xFE;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::paddd(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " paddd " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0xFE;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::paddd(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " paddd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0xFE;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}


void Assembler::psubd(Xmm dst, Xmm src)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " psubd " << dst.name() << ", " << src.name() << "\n";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit())
        *m_bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    *m_bytes << 0x0F << 0xFA;
    *m_bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::psubd(Xmm reg, Mem128 mem)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " psubd " << reg.name() << ", [" << (void*)mem.addr << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit()) *m_bytes << Rex(0, reg.prefix_bit(), 0, 0);
    *m_bytes << 0x0F << 0xFA;
    *m_bytes << ModRM(b00, reg.code(), b101);
    *m_bytes << Rip32(mem.addr, m_bytes->codeEnd() + 4);
}


void Assembler::psubd(Xmm reg, Base base, Disp8 disp)
{
#ifdef R64FX_DEBUG_JIT_STDOUT
    std::cout << (void*)ip() << " psubd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], ";
#endif//R64FX_DEBUG_JIT_STDOUT

    *m_bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit())
        *m_bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    *m_bytes << 0x0F << 0xFA;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(m_bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(m_bytes, reg, base, disp);
}

}//namespace r64fx

