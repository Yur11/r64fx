#include "jit.hpp"
#include <unistd.h>
#include <malloc.h>
#include <sys/mman.h>
#include <iostream>
#include <limits>


using namespace std;

namespace r64fx{


void* alloc_pages_raw(int npages)
{
    return memalign(getpagesize(), npages * getpagesize());
}


void* alloc_alligned_raw(int alignment, int nbytes)
{
    return memalign(alignment, nbytes);
}
    
    
CodeBuffer::CodeBuffer(int npages) : _npages(npages)
{
    begin = end = alloc_pages<unsigned char*>(npages);
    mprotect(begin, getpagesize() * npages, PROT_READ | PROT_WRITE | PROT_EXEC);
}


void CodeBuffer::cleanup()
{
    free(begin);
}


CodeBuffer &CodeBuffer::operator<<(unsigned char byte)
{
    *end = byte;
    end += 1;
    return *this;
}


CodeBuffer &CodeBuffer::operator<<(Imm32 imm)
{
    for(int i=0; i<4; i++)
    {
        *end = imm.bytes.byte[i];
        end += 1;
    }
    return *this;
}


CodeBuffer &CodeBuffer::operator<<(Imm64 imm)
{
    for(int i=0; i<8; i++)
    {
        *end = imm.bytes.byte[i];
        end += 1;
    }
    return *this;
}


CodeBuffer &CodeBuffer::operator<<(Imm16 imm)
{
    for(int i=0; i<2; i++)
    {
        *end = imm.bytes.byte[i];
        end += 1;
    }
    return *this;
}


#ifdef DEBUG
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
#endif//DEBUG


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


struct Vex2{
    unsigned char bytes[2];

    Vex2(bool R, unsigned char ww, bool L, unsigned char pp)
    {
        bytes[0] = 0xC5;
        bytes[1] = ((!R)<<7) | ((~ww)<<3) | (L<<2) | pp;
    }
};


struct Vex3{
    unsigned char bytes[3];

    Vex3(bool R, bool X, bool B, unsigned char map_select, bool W, unsigned char ww, bool L, unsigned char pp)
    {
        bytes[0] = 0xC4;
        bytes[1] = ((!R)<<7) | ((!X)<<6) | ((!B)<<5) | map_select;
        bytes[2] = ((!W)<<7) | ((~ww)<<3) | (L<<2) | pp;
    }
};


struct Rip32{
    int displacement;

    Rip32(long int addr, unsigned char* next_ip)
    {
        long int _addr = (long int) addr;
        long int _next_ip = (long int) next_ip;

        long int _displacement = _addr - _next_ip;

        displacement = (int) _displacement;
    }
};


CodeBuffer &operator<<(CodeBuffer &buff, Rip32 rip)
{
    return buff << Imm32(rip.displacement);
}


CodeBuffer &operator<<(CodeBuffer &buff, Vex2 vex)
{
    for(int i=0; i<2; i++)
    {
        buff << vex.bytes[i];
    }
    return buff;
}


CodeBuffer &operator<<(CodeBuffer &buff, Vex3 vex)
{
    for(int i=0; i<3; i++)
    {
        buff << vex.bytes[i];
    }
    return buff;
}


void encode_modrm_sib_base_and_disp8(CodeBuffer &bytes, Register &reg, Base &base, Disp8 disp)
{
    bytes << ModRM(b01, reg.code(), b100);
    bytes << SIB(b00, b100, base.reg.code());
    bytes << Imm8(disp.byte);
}


void encode_modrm_and_sib_base(CodeBuffer &bytes, Register &reg, Base &base)
{
    if((base.reg.code() & b0111) == b101)
    {
        encode_modrm_sib_base_and_disp8(bytes, reg, base, Disp8(0));
    }
    else
    {
        bytes << ModRM(b00, reg.code(), b100);
        bytes << SIB(b00, b100, base.reg.code());
    }
}


unsigned char shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3)
{
    return (s3 << 6) + (s2 << 4) + (s1 << 2) + s0;
}


#ifdef DEBUG
const char* CmpCode::names[] = {
    "eq", "lt", "le", "unord", "neq", "nlt", "nle", "ord"
};
#endif//DEBUG


void Assembler::mov(GPR64 reg, unsigned long int imm)
{
    if(imm < numeric_limits<unsigned long int>::max())
        mov(reg, Imm32(imm));
    else
        mov(reg, Imm64(imm));
}


void Assembler::mov(GPR32 reg, Mem32 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x8B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(Mem32 mem, GPR32 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x89;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(GPR32 dst, GPR32 src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(src.prefix_bit() || dst.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x8B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::mov(GPR64 reg, Mem64 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x8B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(Mem64 mem, GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x89;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(GPR64 dst, GPR64 src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x8B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::mov(GPR64 reg, Imm32 imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << reg.name() << ", " << (int)imm << "\n";
#endif//DEBUG
    
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << 0xC7;
    bytes << ModRM(b11, 0, reg.code());
    bytes << imm;
}


void Assembler::mov(GPR64 reg, Imm64 imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << reg.name() << ", " << (signed long int) imm << "\n";
#endif//DEBUG
    
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0xB8 + (reg.code() & b0111)) << imm;
}


void Assembler::mov(GPR64 reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    " << reg.name() << ", [" << base.reg.name() << "]\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x8B;    
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::mov(Base base, Disp8 disp, GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    [" << base.reg.name() << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x89;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::mov(Base base, GPR32 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    mov    [" << base.reg.name() << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x89;
    encode_modrm_and_sib_base(bytes, reg, base);    
}


void Assembler::add(GPR32 reg, Mem32 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x03;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(Mem32 mem, GPR32 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    [" << mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x01;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(GPR32 dst, GPR32 src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(src.prefix_bit() || dst.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x03;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::add(GPR64 reg, Mem64 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x03;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(Mem64 mem, GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    [" << mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x01;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(GPR64 dst, GPR64 src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x03;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::add(GPR64 reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    " << reg.name() << ", [" << base.reg.name() << "]\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x03;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);    
}


void Assembler::add(Base base, GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    [" << base.reg.name() << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x01;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::add(GPR64 reg, Imm32 imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    add    " << reg.name() << ", " << (int)imm << "\n";
#endif//DEBUG
    
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << 0x81;
    bytes << ModRM(b11, 0, reg.code());
    bytes << imm;
}


void Assembler::sub(GPR32 reg, Mem32 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x2B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(Mem32 mem, GPR32 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x29;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(GPR32 dst, GPR32 src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(src.prefix_bit() || dst.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x2B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sub(GPR64 reg, Mem64 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x2B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(Mem64 mem, GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x29;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(GPR64 dst, GPR64 src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x2B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sub(GPR64 reg, Imm32 imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    " << reg.name() << ", " << (int)imm << "\n";
#endif//DEBUG
    
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << 0x81;
    bytes << ModRM(b11, 5, reg.code());
    bytes << imm;
}


void Assembler::sub(GPR64 reg, Base base)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    " << reg.name() << ", [" << base.reg.name() << "]\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x2B;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::sub(Base base, GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    sub    [" << base.reg.name() << "], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x29;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::jmp(Mem8 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    jnz   " << (void*)mem.addr << "\n";
#endif//DEBUG
    
    bytes << 0xE9;
    bytes << Rip32(mem.addr, ip() + 4);
}


void Assembler::jnz(Mem8 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    jnz    " << (void*)mem.addr << "\n";
#endif//DEBUG
    
    bytes << 0x0F << 0x85;
    bytes << Rip32(mem.addr, ip() + 4);
}


void Assembler::jz(Mem8 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    jz    " << (void*)mem.addr << "\n";
#endif//DEBUG
    
    bytes << 0x0F << 0x84;
    bytes << Rip32(mem.addr, ip() + 4);
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
#ifdef DEBUG
    dump << (void*)ip() << "    jl   " << (void*)mem.addr << "\n";
#endif//DEBUG
    
    bytes << 0x0F << 0x8C;
    bytes << Rip32(mem.addr, ip() + 4);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << "  " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << second_opcode;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << "  " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << second_opcode;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sse_ps_instruction(unsigned char second_opcode, Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << "  " << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << second_opcode;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


#ifdef DEBUG
#define DUMP_IP_AND_NAME(name) dump << (void*)ip() << "    " << #name;
#else
#define DUMP_IP_AND_NAME(name)
#endif//DEBUG


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
#ifdef DEBUG
    dump << "  " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << third_opcode;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm reg, Mem32 mem)
{
#ifdef DEBUG
    dump << "  " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << third_opcode;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sse_ss_instruction(unsigned char third_opcode, Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << "  " << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(reg.prefix_bit() || base.reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << third_opcode;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
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
#ifdef DEBUG
    dump << (void*)ip() << "    cmp" << kind.name() << "ps    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b11, dst.code(), src.code());
    bytes << kind.code();
}


/* Segfaults! */
void Assembler::cmpps(CmpCode kind, Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cmp" << kind.name() << "ps    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 5);
    bytes << kind.code();
}


void Assembler::cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cmp" << kind.name() << "ps    " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int)disp.byte << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0xC2;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
    bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cmp" << kind.name() << "ss    " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b11, dst.code(), src.code());
    bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cmp" << kind.name() << "ss    " << reg.name() << ", [" << mem.addr << "]\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
    bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cmp" << kind.name() << "ss    " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int)disp.byte << "]\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(reg.prefix_bit() || base.reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0xC2;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
    bytes << kind.code();
}


void Assembler::movups(Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x10;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::movups(Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x10;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movups(Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups" << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x10;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movups(Mem128 mem, Xmm reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x11;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movups(Base base, Disp8 disp, Xmm reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x11;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movaps(Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x28;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::movaps(Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movaps " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x28;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movaps(Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups" << reg.name() << ", [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ]\n";
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x28;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movaps(Mem128 mem, Xmm reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movaps [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x29;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movaps(Base base, Disp8 disp, Xmm reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movups [ " << base.reg.name() << " + " << (unsigned int) disp.byte << " ], " << reg.name() << "\n";
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x29;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movss(Xmm reg, Mem32 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movss " << reg.name() << ", [" << (void*)mem.addr << "]\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x10;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movss(Mem32 mem, Xmm reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    movss [" << (void*)mem.addr << "], " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << 0xF3;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x11;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


#ifdef DEBUG
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
#endif//DEBUG

void Assembler::shufps(Xmm dst, Xmm src, unsigned char imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    shufps " << dst.name() << ", " << src.name() << ", "; 
    dump << "(" << ShufByte(imm) <<  ")\n";
#endif//DEBUG
    
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xC6;
    bytes << ModRM(b11, dst.code(), src.code());
    bytes << imm;
}


void Assembler::shufps(Xmm reg, Mem128 mem, unsigned char imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    shufps " << reg.name() << ", [" << (void*)mem.addr << "], "; 
    dump << "(" << ShufByte(imm) <<  ")\n";
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xC6;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 5);
    bytes << imm;
}


void Assembler::pshufd(Xmm dst, Xmm src, unsigned char imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    pshufd " << dst.name() << ", " << src.name() << ", ";
    dump << "(" << ShufByte(imm) <<  ")\n";
#endif//DEBUG
    
    bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x70;
    bytes << ModRM(b11, dst.code(), src.code());
    bytes << imm;
}


void Assembler::pshufd(Xmm reg, Mem128 mem, unsigned char imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    pshufd " << reg.name() << ", [" << (void*)mem.addr << "], "; 
    dump << "(" << ShufByte(imm) <<  ")\n";
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x70;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 5);
    bytes << imm;
}


void Assembler::pshufd(Xmm reg, Base base, Disp8 disp, unsigned char imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    pshufd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], "; 
    dump << "(" << ShufByte(imm) <<  ")\n";
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit())
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x70;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
    bytes << imm;
}


void Assembler::cvtps2dq(Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cvtps2dq " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit())
        bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x5B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::cvtps2dq(Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cvtps2dq " << reg.name() << ", [" << (void*)mem.addr << "], "; 
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x5B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::cvtps2dq(Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cvtps2dq " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], "; 
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x5B;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::cvtdq2ps(Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cvtdq2ps " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    if(dst.prefix_bit() || src.prefix_bit())
        bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x5B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::cvtdq2ps(Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cvtdq2ps " << reg.name() << ", [" << (void*)mem.addr << "], "; 
#endif//DEBUG
    
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x5B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::cvtdq2ps(Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cvtdq2ps " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], "; 
#endif//DEBUG
    
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x5B;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::paddd(Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    paddd " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit())
        bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xFE;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::paddd(Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    paddd " << reg.name() << ", [" << (void*)mem.addr << "], "; 
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xFE;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::paddd(Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    paddd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], "; 
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0xFE;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::psubd(Xmm dst, Xmm src)
{
#ifdef DEBUG
    dump << (void*)ip() << "    psubd " << dst.name() << ", " << src.name() << "\n";
#endif//DEBUG
    
    bytes << 0x66;
    if(dst.prefix_bit() || src.prefix_bit())
        bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xFA;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::psubd(Xmm reg, Mem128 mem)
{
#ifdef DEBUG
    dump << (void*)ip() << "    psubd " << reg.name() << ", [" << (void*)mem.addr << "], "; 
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xFA;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip32(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::psubd(Xmm reg, Base base, Disp8 disp)
{
#ifdef DEBUG
    dump << (void*)ip() << "    psubd " << reg.name() << ", [" << base.reg.name() << " + " << (unsigned int) disp.byte << "], "; 
#endif//DEBUG
    
    bytes << 0x66;
    if(reg.prefix_bit() || base.reg.prefix_bit()) 
        bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0xFA;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::push(GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    push  " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0x50 + (reg.code() & b0111));
}


void Assembler::pop(GPR64 reg)
{
#ifdef DEBUG
    dump << (void*)ip() << "    pop   " << reg.name() << "\n";
#endif//DEBUG
    
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0x58 + (reg.code() & b0111));
}


void Assembler::cmp(GPR64 reg, Imm32 imm)
{
#ifdef DEBUG
    dump << (void*)ip() << "    cmp   " << reg.name() << ", " << (int)imm << "\n";
#endif//DEBUG
    
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x81;
    bytes << ModRM(b11, 7, reg.code());
    bytes << imm;
}

}//namespace r64fx

