#include "jit.h"
#include <unistd.h>
#include <malloc.h>
#include <sys/mman.h>
#include <iostream>


using namespace std;

namespace r64fx{


CodeBuffer::CodeBuffer(int npages) : _npages(npages)
{
    begin = end = (unsigned char*) memalign(getpagesize(), npages * getpagesize());
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


void CodeBuffer::allowExecution()
{
    mprotect(begin, getpagesize() * npages(), PROT_EXEC);
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


struct Rip{
    int displacement;

    Rip(long int addr, unsigned char* next_ip)
    {
        long int _addr = (long int) addr;
        long int _next_ip = (long int) next_ip;

        long int _displacement = _addr - _next_ip;

        displacement = (int) _displacement;
    }
};


CodeBuffer &operator<<(CodeBuffer &buff, Rip rip)
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


void Assembler::add(GPR32 reg, Mem32 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x03;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(Mem32 mem, GPR32 reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x01;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(GPR32 dst, GPR32 src)
{
    if(src.prefix_bit() || dst.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x03;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::add(GPR64 reg, Mem64 mem)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x03;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(Mem64 mem, GPR64 reg)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x01;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::add(GPR64 dst, GPR64 src)
{
    bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x03;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::add(GPR64 reg, Base base)
{
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x03;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::add(Base base, GPR64 reg)
{
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x01;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::sub(GPR32 reg, Mem32 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x2B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(Mem32 mem, GPR32 reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x29;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(GPR32 dst, GPR32 src)
{
    if(src.prefix_bit() || dst.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x2B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sub(GPR64 reg, Mem64 mem)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x2B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(Mem64 mem, GPR64 reg)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x29;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::sub(GPR64 dst, GPR64 src)
{
    bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x2B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::sub(GPR64 reg, Imm32 imm)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x2D;
    bytes << imm;
}


void Assembler::sub(GPR64 reg, Base base)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x2B;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::sub(Base base, GPR64 reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x29;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::mov(GPR32 reg, Mem32 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x8B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(Mem32 mem, GPR32 reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, 1, 0, 0);
    bytes << 0x89;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(GPR32 dst, GPR32 src)
{
    if(src.prefix_bit() || dst.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x8B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::mov(GPR64 reg, Mem64 mem)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x8B;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(Mem64 mem, GPR64 reg)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x89;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::mov(GPR64 dst, GPR64 src)
{
    bytes << Rex(1, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x8B;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::mov(GPR64 reg, Imm32 imm)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << 0xC7;
    bytes << ModRM(b11, 0, reg.code());
    bytes << imm;
}


void Assembler::mov(GPR64 reg, Imm64 imm)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0xB8 + (reg.code() & b0111)) << imm;
}


void Assembler::mov(GPR64 reg, Base base)
{
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x8B;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::mov(Base base, GPR64 reg)
{
    bytes << Rex(1, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x89;
    encode_modrm_and_sib_base(bytes, reg, base);
}


void Assembler::push(GPR64 reg)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0x50 + (reg.code() & b0111));
}


void Assembler::pop(GPR64 reg)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0x58 + (reg.code() & b0111));
}


void Assembler::cmp(GPR64 reg, Imm32 imm)
{
    bytes << Rex(1, reg.prefix_bit(), 0, 0);
    bytes << 0x81;
    bytes << ModRM(b11, 7, reg.code());
    bytes << imm;
}


void Assembler::jnz(Mem8 mem)
{
    auto next_addr = ((long int) ip()) + 6;
    auto target_addr = (long int) mem.addr;
    auto offset = target_addr - next_addr;
    bytes << 0x0F << 0x85;
    bytes << Imm32(offset);
}


#define ENCODE_SSE_PS_INSTRUCTION(name, second_opcode)\
void Assembler::name(Xmm dst, Xmm src)\
{\
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());\
    bytes << 0x0F << second_opcode;\
    bytes << ModRM(b11, dst.code(), src.code());\
}\
\
\
void Assembler::name(Xmm reg, Mem128 mem)\
{\
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);\
    bytes << 0x0F << second_opcode;\
    bytes << ModRM(b00, reg.code(), b101);\
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);\
}\
\
\
void Assembler::name(Xmm reg, Base base, Disp8 disp)\
{\
    if(reg.prefix_bit() || base.reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());\
    bytes << 0x0F << second_opcode;\
    if(disp.byte == 0)\
        encode_modrm_and_sib_base(bytes, reg, base);\
    else\
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);\
}\


#define ENCODE_SSE_SS_INSTRUCTION(name, third_opcode)\
void Assembler::name(Xmm dst, Xmm src)\
{\
    bytes << 0xF3;\
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());\
    bytes << 0x0F << third_opcode;\
    bytes << ModRM(b11, dst.code(), src.code());\
}\
\
\
void Assembler::name(Xmm reg, Mem32 mem)\
{\
    bytes << 0xF3;\
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);\
    bytes << 0x0F << third_opcode;\
    bytes << ModRM(b00, reg.code(), b101);\
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);\
}\
\
\
void Assembler::name(Xmm reg, Base base, Disp8 disp)\
{\
    bytes << 0xF3;\
    if(reg.prefix_bit() || base.reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());\
    bytes << 0x0F << third_opcode;\
    if(disp.byte == 0)\
        encode_modrm_and_sib_base(bytes, reg, base);\
    else\
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);\
}\


#define ENCODE_SSE_INSTRUCTION(name, opcode)\
    ENCODE_SSE_PS_INSTRUCTION(name##ps, opcode)\
    ENCODE_SSE_SS_INSTRUCTION(name##ss, opcode)


/* *ss instructions broken! Rex bit seems to be leaking!*/
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
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b11, dst.code(), src.code());
    bytes << kind.code();
}


void Assembler::cmpps(CmpCode kind, Xmm reg, Mem128 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
    bytes << kind.code();
}


void Assembler::cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
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
    bytes << 0xF3;
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b11, dst.code(), src.code());
    bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Mem128 mem)
{
    bytes << 0xF3;
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0xC2;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
    bytes << kind.code();
}


void Assembler::cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp)
{
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
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x10;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::movups(Xmm reg, Mem128 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x10;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movups(Xmm reg, Base base, Disp8 disp)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x10;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movups(Mem128 mem, Xmm reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x11;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movups(Base base, Disp8 disp, Xmm reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x11;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movaps(Xmm dst, Xmm src)
{
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x28;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::movaps(Xmm reg, Mem128 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x28;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movaps(Xmm reg, Base base, Disp8 disp)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x28;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}


void Assembler::movaps(Mem128 mem, Xmm reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x29;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}


void Assembler::movaps(Base base, Disp8 disp, Xmm reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, base.reg.prefix_bit());
    bytes << 0x0F << 0x29;
    if(disp.byte == 0)
        encode_modrm_and_sib_base(bytes, reg, base);
    else
        encode_modrm_sib_base_and_disp8(bytes, reg, base, disp);
}

}//namespace r64fx

