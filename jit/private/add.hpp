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