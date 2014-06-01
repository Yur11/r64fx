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