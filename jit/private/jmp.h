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