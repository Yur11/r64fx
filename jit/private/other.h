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