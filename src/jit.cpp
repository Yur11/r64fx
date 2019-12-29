#include "jit.hpp"
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>

#include <iostream>

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


void AssemblerBuffer::permitExecution()
{
    mprotect(begin(), nbytes(), PROT_READ | PROT_WRITE | PROT_EXEC);
}


void AssemblerBuffer::prohibitExecution()
{
    mprotect(begin(), nbytes(), PROT_READ | PROT_WRITE);
}


void AssemblerBuffer::fill(unsigned char byte, int nbytes)
{
    auto p = MemoryBuffer::grow(nbytes);
    for(int i=0; i<nbytes; i++)
        p[i] = byte;
}


void AssemblerBuffer::write(unsigned char byte)
{
    auto p = MemoryBuffer::grow(1);
    p[0] = byte;
}


void AssemblerBuffer::write(unsigned char byte0, unsigned char byte1)
{
    auto p = MemoryBuffer::grow(2);
    p[0] = byte0;
    p[1] = byte1;
}


void AssemblerBuffer::write(unsigned char opcode, Register reg, Imm64 imm)
{
    auto p = MemoryBuffer::grow(10);
    p[0] = Rex(1, 0, 0, reg.prefixBit());
    p[1] = opcode + (reg.lowerBits());
    for(int i=0; i<8; i++)
        p[i + 2] = imm.b[i];
}


void AssemblerBuffer::write(unsigned char opcode, Register reg)
{
    auto p = MemoryBuffer::grow(2);
    p[0] = Rex(1, 0, 0, reg.prefixBit());
    p[1] = opcode + (reg.lowerBits());
}


void AssemblerBuffer::write(unsigned char opcode1, unsigned char opcode2, JumpLabel &label)
{
    int nbytes = 5;
    if(opcode1)
        nbytes++;

    auto p = MemoryBuffer::grow(nbytes);

    int r = 0;
    if(opcode1)
        p[r++] = opcode1;
    p[r++] = opcode2;

    Imm32 imm = Imm32(0);
    if(label.jmpAddr())
    {
        imm = Rip((unsigned long)(MemoryBuffer::begin() + label.jmpAddr()), MemoryBuffer::ptr());
    }
    else
    {
        long offset = p - MemoryBuffer::begin() + r;
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


void AssemblerBuffer::write(const Opcode &opcode, const Operands &operands)
{
    auto p = MemoryBuffer::grow(opcode.byteCount() + operands.totalByteCount());
    int r = 0;
    if(opcode.has66())
        p[r++] = 0x66;
    if(opcode.hasF2())
        p[r++] = 0xF2;
    if(opcode.hasF3())
        p[r++] = 0xF3;
    if(operands.hasRex())
        p[r++] = 64 | operands.wrxb();
    if(opcode.has0F())
        p[r++] = 0x0F;
    p[r++] = opcode.byte1();
    if(opcode.hasByte2())
        p[r++] = opcode.byte2();
    auto operand_bytes = operands.operandBytes();
    for(int i=0; i<operands.operandByteCount(); i++)
    {
        p[r++] = operand_bytes & 0xFF; operand_bytes >>= 8;
    }
}


void AssemblerBuffer::write(const OpPref2 &pref, unsigned char opcode, const Operands &operands)
{
    auto p = MemoryBuffer::grow(3 + operands.totalByteCount());
    int r = 0;
    p[r++] = pref.b[0];
    p[r++] = pref.b[1];
    p[r++] = opcode;
    auto operand_bytes = operands.operandBytes();
    for(int i=0; i<operands.operandByteCount(); i++)
    {
        p[r++] = operand_bytes & 0xFF; operand_bytes >>= 8;
    }
}


void AssemblerBuffer::write(const OpPref3 &pref, unsigned char opcode, const Operands &operands)
{
    auto p = MemoryBuffer::grow(4 + operands.totalByteCount());
    int r = 0;
    p[r++] = pref.b[0];
    p[r++] = pref.b[1];
    p[r++] = pref.b[2];
    p[r++] = opcode;
    auto operand_bytes = operands.operandBytes();
    for(int i=0; i<operands.operandByteCount(); i++)
    {
        p[r++] = operand_bytes & 0xFF; operand_bytes >>= 8;
    }
}


void AssemblerBuffer::markLabel(JumpLabel &label)
{
    if(label.immAddr())
    {
        R64FX_DEBUG_ASSERT(label.jmpAddr() == 0);
        unsigned char* imm = begin() + label.immAddr();
        Imm32 rip = Rip((unsigned long)ptr(), imm + 4);
        for(int i=0; i<4; i++)
        {
            imm[i] = rip.b[i];
        }
    }

    long offset = bytesUsed();
    R64FX_DEBUG_ASSERT(offset <= 0x7FFFFFFF || offset >= -0x7FFFFFFF);
    label.setJmpAddr(int(offset));
}

}//namespace r64fx

