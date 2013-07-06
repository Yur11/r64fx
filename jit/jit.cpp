#include "jit.h"
#include <iostream>

namespace r64fx{

inline unsigned char Disp8(unsigned char disp) { return disp; }

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


void Assembler::add(GPR32 reg, unsigned int imm)
{
    if(reg.prefix_bit()) bytes << Rex(0, 0, 0, 1);
    bytes << 0x81;
    bytes << ModRM(b11, 0x0, reg.code());
    bytes << Imm32(imm);
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


void Assembler::add(GPR64 reg, unsigned int imm)
{
    bytes << Rex(0, 0, 0, reg.prefix_bit());
    bytes << 0x81;
    bytes << ModRM(b11, 0x0, reg.code());
    bytes << Imm32(imm);
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


void Assembler::mov(GPR32 reg, unsigned int imm)
{
    if(reg.prefix_bit()) bytes << Rex(0, 0, 0, 1);
    bytes << (0xB8 + (reg.code() & b0111)) << Imm32(imm);
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
    if(src.code() == b0101 || src.code() == b1101)
        bytes << ModRM(b10, dst.code(), src.code()) << Disp8(0);
    else
        bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::mov(GPR64 reg, unsigned long long imm)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0xB8 + (reg.code() & b0111)) << Imm64(imm);
}


void Assembler::push(GPR64 reg)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0x50 + (reg.code() & b0111));
}


void Assembler::push(unsigned int imm)
{
    bytes << Rex(1, 0, 0, 0);
    if(imm < 256)
    {
        bytes << 0x6A;
        bytes << Imm8(imm);
    }
    else
    {
        bytes << 0x68;
        bytes << Imm32(imm);
    }
}


void Assembler::pop(GPR64 reg)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << (0x58 + (reg.code() & b0111));
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


void Assembler::sub(GPR32 reg, unsigned int imm)
{
    if(reg.prefix_bit()) bytes << Rex(0, 0, 0, 1);
    bytes << 0x81;
    bytes << ModRM(b11, 0x5, reg.code());
    bytes << Imm32(imm);
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


void Assembler::sub(GPR64 reg, unsigned int imm)
{
    bytes << Rex(1, 0, 0, reg.prefix_bit());
    bytes << 0x81;
    bytes << ModRM(b11, 0x5, reg.code());
    bytes << Imm32(imm);
}


void Assembler::addps(Xmm dst, Xmm src)
{
    if(dst.prefix_bit() || src.prefix_bit()) bytes << Rex(0, dst.prefix_bit(), 0, src.prefix_bit());
    bytes << 0x0F << 0x58;
    bytes << ModRM(b11, dst.code(), src.code());
}


void Assembler::addps(Xmm reg, Mem128 mem)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x58;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
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


void Assembler::movups(Mem128 mem, Xmm reg)
{
    if(reg.prefix_bit()) bytes << Rex(0, reg.prefix_bit(), 0, 0);
    bytes << 0x0F << 0x11;
    bytes << ModRM(b00, reg.code(), b101);
    bytes << Rip(mem.addr, bytes.codeEnd() + 4);
}

}//namespace r64fx

