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