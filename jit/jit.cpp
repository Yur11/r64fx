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

#include "private/mov.hpp"
#include "private/add.hpp"
#include "private/sub.hpp"
#include "private/jmp.hpp"
#include "private/sse.hpp"
#include "private/other.hpp"


}//namespace r64fx

