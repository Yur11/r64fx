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