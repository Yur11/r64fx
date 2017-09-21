#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#ifdef R64FX_JIT_DEBUG_STDOUT
#include <iostream>
#include <string>
#endif//R64FX_JIT_DEBUG_STDOUT

#include "MemoryUtils.hpp"
#include "binary_constants.hpp"

namespace r64fx{

union Imm8{
    signed char    s;
    unsigned char  u;
};

inline Imm8 Imm8S(signed char byte)
{
    Imm8 imm;
    imm.s = byte;
    return imm;
}

inline Imm8 Imm8U(unsigned char byte)
{
    Imm8 imm;
    imm.u = byte;
    return imm;
}


union Imm16{
    signed short    s;
    unsigned short  u;
    unsigned char   b[2];
};

inline Imm16 Imm16S(signed short word)
{
    Imm16 imm;
    imm.s = word;
    return imm;
}

inline Imm16 Imm16U(unsigned short word)
{
    Imm16 imm;
    imm.u = word;
    return imm;
}


union Imm32{
    signed int     s;
    unsigned int   u;
    unsigned char  b[4];
};

inline Imm32 Imm32S(signed int dword)
{
    Imm32 imm;
    imm.s = dword;
    return imm;
}

inline Imm32 Imm32U(unsigned int dword)
{
    Imm32 imm;
    imm.u = dword;
    return imm;
}


union Imm64{
    signed long    s;
    unsigned long  u;
    unsigned char  b[8];
};

inline Imm64 Imm64S(signed long qword)
{
    Imm64 imm;
    imm.s = qword;
    return imm;
}

inline Imm64 Imm64U(unsigned long qword)
{
    Imm64 imm;
    imm.u = qword;
    return imm;
}

inline Imm64 ImmAddr(void* addr)
{
    return Imm64U((unsigned long)addr);
}

class Register{
    const unsigned char mbits;

public:
    Register(const unsigned char bits) : mbits(bits) {}

    inline unsigned char bits() const { return mbits; }

    inline unsigned char code() const { return mbits; }

    /* R or B bit of the REX prefix.*/
    inline bool prefix_bit() const { return mbits & b1000; }
};

class GPR64 : public Register{
#ifdef R64FX_JIT_DEBUG_STDOUT
    static const char* names[];
#endif//R64FX_JIT_DEBUG_STDOUT

public:
    explicit GPR64(const unsigned char bits) : Register(bits) {}

#ifdef R64FX_JIT_DEBUG_STDOUT
    inline std::string name() const { return names[Register::code()]; }
#endif//R64FX_JIT_DEBUG_STDOUT
};

const GPR64
    rax(b0000),
    rcx(b0001),
    rdx(b0010),
    rbx(b0011),
    rsp(b0100),
    rbp(b0101),
    rsi(b0110),
    rdi(b0111),
    r8 (b1000),
    r9 (b1001),
    r10(b1010),
    r11(b1011),
    r12(b1100),
    r13(b1101),
    r14(b1110),
    r15(b1111)
;


class Xmm : public Register{
#ifdef R64FX_JIT_DEBUG_STDOUT
    static const char* names[];
#endif//R64FX_JIT_DEBUG_STDOUT

public:
    explicit Xmm(const unsigned char bits) : Register(bits) {}

#ifdef R64FX_JIT_DEBUG_STDOUT
    inline std::string name() const { return names[Register::code()]; }
#endif//R64FX_JIT_DEBUG_STDOUT
};

const Xmm
    xmm0(b0000),
    xmm1(b0001),
    xmm2(b0010),
    xmm3(b0011),
    xmm4(b0100),
    xmm5(b0101),
    xmm6(b0110),
    xmm7(b0111),
    xmm8(b1000),
    xmm9(b1001),
    xmm10(b1010),
    xmm11(b1011),
    xmm12(b1100),
    xmm13(b1101),
    xmm14(b1110),
    xmm15(b1111)
;


#ifdef R64FX_DEBUG
#define R64FX_JIT_DEBUG_MEM_ALIGN(bytes) assert((long int)addr % bytes == 0);
#endif//R64FX_DEBUG

struct Mem8{
    long int m_addr = 0;

public:
    Mem8(){}
    Mem8(void* addr){ m_addr = (long int) addr; }

    inline long int addr() const { return m_addr; }
};

struct Mem16 : public Mem8{
    Mem16(){}
    Mem16(void* addr){ R64FX_JIT_DEBUG_MEM_ALIGN(2); m_addr = (unsigned long)addr; }
};

struct Mem32 : public Mem16{
    Mem32(){}
    Mem32(void* addr){ R64FX_JIT_DEBUG_MEM_ALIGN(4); m_addr = (unsigned long)addr; }
};

struct Mem64 : public Mem32{
    Mem64(){}
    Mem64(void* addr){ R64FX_JIT_DEBUG_MEM_ALIGN(8); m_addr = (unsigned long)addr; }
};

struct Mem128 : public Mem64{
    Mem128(){}
    Mem128(void* addr){ R64FX_JIT_DEBUG_MEM_ALIGN(16); m_addr = (unsigned long)addr; }
};

#undef R64FX_JIT_DEBUG_MEM_ALIGN


struct Base{
    GPR64 reg;

    explicit Base(GPR64 reg) : reg(reg) {}
};


struct Disp8{
    unsigned char byte;

    explicit Disp8(unsigned char byte) : byte(byte) {}
};


class CmpCode{
    unsigned int m_code;

#ifdef R64FX_JIT_DEBUG_STDOUT
    static const char* names[];
#endif//R64FX_JIT_DEBUG_STDOUT

public:
    CmpCode(unsigned int code) : m_code(code) {}

    inline unsigned int code() const { return m_code; }

#ifdef R64FX_JIT_DEBUG_STDOUT
    inline std::string name() const { return names[code()]; }
#endif//R64FX_JIT_DEBUG_STDOUT
};

/* Codes used with cmpps. */
const CmpCode
    EQ(0),
    LT(1),
    LE(2),
    UNORD(3),
    NEQ(4),
    NLT(5),
    NLE(6),
    ORD(7)
;


/* Pack four 0..3 values into a sigle byte. To be used with shuffle instructions.

    Each parameter defines the source scalar from which to read the data into the current position.
    For example
       Setting s3 to 0, means moving the first scalar form the source vector into the last scalar of the destination vector.
       - shuf(0, 1, 2, 3) - no shuffle.
       - shuf(3, 2, 1, 0) - reverse order.
       - etc...
 */
class Shuf{
    unsigned char m_byte = 0;

public:
    Shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3)
    {
        m_byte = (s3 << 6) + (s2 << 4) + (s1 << 2) + s0;
    }

    inline unsigned char byte() const { return m_byte; }

    inline int s0() const { return m_byte & b11; }
    inline int s1() const { return (m_byte >> 2) & b11; }
    inline int s2() const { return (m_byte >> 4) & b11; }
    inline int s3() const { return (m_byte >> 6) & b11; }

#ifdef R64FX_JIT_DEBUG_STDOUT
    inline std::string str() const
    {
        return "{"
            + std::string(1, s0() + '0') + ", "
            + std::string(1, s1() + '0') + ", "
            + std::string(1, s2() + '0') + ", "
            + std::string(1, s3() + '0') + "}\n";
    }
#endif//R64FX_JIT_DEBUG_STDOUT
};


class Assembler{
    unsigned char*  m_begin       = nullptr;
    unsigned char*  m_end         = nullptr;
    unsigned long   m_size        = 0;
    unsigned long   m_page_count  = 0;

public:
    Assembler(unsigned long page_count = 1) { resize(page_count); }

    ~Assembler() { resize(0); }

    void resize(unsigned long page_count);

    inline void rewind()
    {
        m_end = m_begin;
    }

    /* Pointer to the beginning of the buffer. */
    inline unsigned char* codeBegin() const { return m_begin; }

    /* Pointer to the byte past the end of the written bytes. */
    inline unsigned char* codeEnd() const { return m_end; }

    inline unsigned char* ip() const { return m_end; }

    inline void setCodeEnd(void* addr) { m_end = (unsigned char*) addr; }

    inline unsigned long size() const { return m_size; }

    inline unsigned long bytesUsed() const { return (unsigned long)(m_end - m_begin); }

    inline unsigned long bytesAvailable() const { return size() - bytesUsed(); }

    inline void ensureAvailable(unsigned long nbytes) { while(bytesAvailable() < nbytes) resize(pageCount() + 1); }

    inline unsigned long pageCount() const { return m_page_count; }

private:
    void write_bytes(unsigned char byte, int nbytes);

    void write(unsigned char byte);
    void write(unsigned char byte0, unsigned char byte1);

    void write(unsigned char opcode, unsigned char r, GPR64 reg, Imm32 imm);
    void write(unsigned char opcode, GPR64 reg, Imm64 imm);

    void write(unsigned char opcode, GPR64 dst, GPR64 src);
    void write(unsigned char opcode, GPR64 reg, Mem64 mem);
    void write(unsigned char opcode, GPR64 reg, Base base, Disp8 disp);

    void write(unsigned char opcode, Mem8 mem);
    void write(unsigned char opcode1, unsigned char opcode2, Mem8 mem);

    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm dst, Xmm src, int imm = -1);
    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, Mem8 mem, int imm = -1);
    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, Base base, Disp8 disp, int imm = -1);

    void write(unsigned char opcode, GPR64 reg);

#ifdef R64FX_JIT_DEBUG_STDOUT
#define R64FX_JIT_DEBUG_PRINT(...) { Assembler::print(__VA_ARGS__); }
#else
#define R64FX_JIT_DEBUG_PRINT(...)
#endif//R64FX_JIT_DEBUG_STDOUT

public:
    inline void nop(int count) { R64FX_JIT_DEBUG_PRINT("nop");    write(0x90, count); }
    inline void ret()          { R64FX_JIT_DEBUG_PRINT("ret");    write(0xC3); }
    inline void rdtsc()        { R64FX_JIT_DEBUG_PRINT("rdtsc");  write(0x0F, 0x31); }

    inline void mov(GPR64 reg, Imm32 imm){ R64FX_JIT_DEBUG_PRINT("mov", reg, imm);  write(0xC7, 0, reg, imm); }
    inline void mov(GPR64 reg, Imm64 imm){ R64FX_JIT_DEBUG_PRINT("mov", reg, imm);  write(0xB8, reg, imm); }
    inline void mov(GPR64 dst, GPR64 src){ R64FX_JIT_DEBUG_PRINT("mov", dst, src);  write(0x8B, dst, src); }
    inline void mov(GPR64 reg, Mem64 mem){ R64FX_JIT_DEBUG_PRINT("mov", reg, mem);  write(0x8B, reg, mem); }
    inline void mov(Mem64 mem, GPR64 reg){ R64FX_JIT_DEBUG_PRINT("mov", mem, reg);  write(0x89, reg, mem); }
    inline void mov(GPR64 reg, Base base, Disp8 disp){ R64FX_JIT_DEBUG_PRINT("mov", reg, base, disp);  write(0x8B, reg, base, disp); }
    inline void mov(Base base, Disp8 disp, GPR64 reg){ R64FX_JIT_DEBUG_PRINT("mov", base, disp, reg);  write(0x89, reg, base, disp); }

    inline void add(GPR64 reg, Imm32 imm){ R64FX_JIT_DEBUG_PRINT("add", reg, imm);  write(0x81, 0, reg, imm); }
    inline void add(GPR64 dst, GPR64 src){ R64FX_JIT_DEBUG_PRINT("add", dst, src);  write(0x03, dst, src); }
    inline void add(GPR64 reg, Mem64 mem){ R64FX_JIT_DEBUG_PRINT("add", reg, mem);  write(0x03, reg, mem); }
    inline void add(Mem64 mem, GPR64 reg){ R64FX_JIT_DEBUG_PRINT("add", mem, reg);  write(0x01, reg, mem); }
    inline void add(GPR64 reg, Base base, Disp8 disp){ R64FX_JIT_DEBUG_PRINT("add", reg, base, disp);  write(0x03, reg, base, disp); }
    inline void add(Base base, Disp8 disp, GPR64 reg){ R64FX_JIT_DEBUG_PRINT("add", base, disp, reg);  write(0x01, reg, base, disp); }

    inline void sub(GPR64 reg, Imm32 imm){ R64FX_JIT_DEBUG_PRINT("sub", reg, imm);  write(0x81, 5, reg, imm); }
    inline void sub(GPR64 dst, GPR64 src){ R64FX_JIT_DEBUG_PRINT("sub", dst, src);  write(0x2B, dst, src); }
    inline void sub(GPR64 reg, Mem64 mem){ R64FX_JIT_DEBUG_PRINT("sub", reg, mem);  write(0x2B, reg, mem); }
    inline void sub(Mem64 mem, GPR64 reg){ R64FX_JIT_DEBUG_PRINT("sub", mem, reg);  write(0x29, reg, mem); }
    inline void sub(GPR64 reg, Base base, Disp8 disp){ R64FX_JIT_DEBUG_PRINT("sub", reg, base, disp);  write(0x2B, reg, base, disp); }
    inline void sub(Base base, Disp8 disp, GPR64 reg){ R64FX_JIT_DEBUG_PRINT("sub", base, disp, reg);  write(0x29, reg, base, disp); }

    inline void push (GPR64 reg){ R64FX_JIT_DEBUG_PRINT("push", reg);  write(0x50, reg); }
    inline void pop  (GPR64 reg){ R64FX_JIT_DEBUG_PRINT("pop",  reg);  write(0x58, reg); }

    inline void cmp(GPR64 reg, Imm32 imm){ R64FX_JIT_DEBUG_PRINT("cmp", reg, imm);  write(0x81, 7, reg, imm); }

    inline void jmp (Mem8 mem){ R64FX_JIT_DEBUG_PRINT("jmp", mem);  write(0xE9, mem); }
    inline void jnz (Mem8 mem){ R64FX_JIT_DEBUG_PRINT("jnz", mem);  write(0x0F, 0x85, mem); }
    inline void jz  (Mem8 mem){ R64FX_JIT_DEBUG_PRINT("jz",  mem);  write(0x0F, 0x84, mem); }
    inline void je  (Mem8 mem){ R64FX_JIT_DEBUG_PRINT("je",  mem);  write(0x0F, 0x84, mem); }
    inline void jne (Mem8 mem){ R64FX_JIT_DEBUG_PRINT("jne", mem);  write(0x0F, 0x85, mem); }
    inline void jl  (Mem8 mem){ R64FX_JIT_DEBUG_PRINT("jl",  mem);  write(0x0F, 0x8C, mem); }

    inline void movaps(Xmm dst, Xmm src)    { R64FX_JIT_DEBUG_PRINT("movaps", dst, src);  write0x0F(0, 0x28, dst, src); }
    inline void movaps(Xmm reg, Mem128 mem) { R64FX_JIT_DEBUG_PRINT("movaps", reg, mem);  write0x0F(0, 0x28, reg, mem); }
    inline void movaps(Mem128 mem, Xmm reg) { R64FX_JIT_DEBUG_PRINT("movaps", mem, reg);  write0x0F(0, 0x29, reg, mem); }
    inline void movaps(Xmm reg, Base base, Disp8 disp) { R64FX_JIT_DEBUG_PRINT("movaps", reg, base, disp);  write0x0F(0, 0x28, reg, base, disp); }
    inline void movaps(Base base, Disp8 disp, Xmm reg) { R64FX_JIT_DEBUG_PRINT("movaps", base, disp, reg);  write0x0F(0, 0x29, reg, base, disp); }

    inline void movups(Xmm dst, Xmm src)   { R64FX_JIT_DEBUG_PRINT("movups", dst, src);  write0x0F(0, 0x10, dst, src); }
    inline void movups(Xmm reg, Mem32 mem) { R64FX_JIT_DEBUG_PRINT("movups", reg, mem);  write0x0F(0, 0x10, reg, mem); }
    inline void movups(Mem32 mem, Xmm reg) { R64FX_JIT_DEBUG_PRINT("movups", mem, reg);  write0x0F(0, 0x11, reg, mem); }
    inline void movups(Xmm reg, Base base, Disp8 disp) { R64FX_JIT_DEBUG_PRINT("movups", reg, base, disp);  write0x0F(0, 0x10, reg, base, disp); }
    inline void movups(Base base, Disp8 disp, Xmm reg) { R64FX_JIT_DEBUG_PRINT("movups", base, disp, reg);  write0x0F(0, 0x11, reg, base, disp); }

private:
    inline void sse_ps_instruction(unsigned char second_opcode_byte, Xmm dst, Xmm src)
        { write0x0F(0, second_opcode_byte, dst, src); }

    inline void sse_ps_instruction(unsigned char second_opcode_byte, Xmm reg, Mem128 mem)
        { write0x0F(0, second_opcode_byte, reg, mem); }

    inline void sse_ps_instruction(unsigned char second_opcode_byte, Xmm reg, Base base, Disp8 disp)
        { write0x0F(0, second_opcode_byte, reg, base, disp); }

    inline void sse_ss_instruction(unsigned char third_opcode_byte, Xmm dst, Xmm src)
        { write0x0F(0xF3, third_opcode_byte, dst, src); }

    inline void sse_ss_instruction(unsigned char third_opcode_byte, Xmm reg, Mem32 mem)
        { write0x0F(0xF3, third_opcode_byte, reg, mem); }

    inline void sse_ss_instruction(unsigned char third_opcode_byte, Xmm reg, Base base, Disp8 disp)
        { write0x0F(0xF3, third_opcode_byte, reg, base, disp); }

#define ENCODE_SSE_PS_INSTRUCTION(name, second_opcode_byte)\
inline void name(Xmm dst, Xmm src)\
    { R64FX_JIT_DEBUG_PRINT(#name, dst, src); sse_ps_instruction(second_opcode_byte, dst, src); }\
\
inline void name(Xmm reg, Mem128 mem)\
 { R64FX_JIT_DEBUG_PRINT(#name, reg, mem); sse_ps_instruction(second_opcode_byte, reg, mem); }\
\
inline void name(Xmm reg, Base base, Disp8 disp)\
 { R64FX_JIT_DEBUG_PRINT(#name, reg, base, disp); sse_ps_instruction(second_opcode_byte, reg, base, disp); }

#define ENCODE_SSE_SS_INSTRUCTION(name, third_opcode_byte)\
inline void name(Xmm dst, Xmm src)\
    { R64FX_JIT_DEBUG_PRINT(#name, dst, src); sse_ss_instruction(third_opcode_byte, dst, src); }\
\
inline void name(Xmm reg, Mem32 mem)\
    { R64FX_JIT_DEBUG_PRINT(#name, reg, mem); sse_ss_instruction(third_opcode_byte, reg, mem); }\
\
inline void name(Xmm reg, Base base, Disp8 disp)\
    { R64FX_JIT_DEBUG_PRINT(#name, reg, base, disp); sse_ss_instruction(third_opcode_byte, reg, base, disp); }

#define ENCODE_SSE_INSTRUCTION(name, opcode)\
    ENCODE_SSE_PS_INSTRUCTION(name##ps, opcode)\
    ENCODE_SSE_SS_INSTRUCTION(name##ss, opcode)

public:
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

    inline void cmpps(CmpCode kind, Xmm dst, Xmm src)
        { R64FX_JIT_DEBUG_PRINT("cmp"+kind.name()+"ps", dst, src);  write0x0F(0, 0xC2, dst, src, kind.code()); }

    inline void cmpps(CmpCode kind, Xmm reg, Mem128 mem)
        { R64FX_JIT_DEBUG_PRINT("cmp"+kind.name()+"ps", reg, mem);  write0x0F(0, 0xC2, reg, mem, kind.code()); }

    inline void cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp)
        { R64FX_JIT_DEBUG_PRINT("cmp"+kind.name()+"ps", reg, base, disp);  write0x0F(0, 0xC2, reg, base, disp, kind.code()); }

    inline void cmpss(CmpCode kind, Xmm dst, Xmm src)
        { R64FX_JIT_DEBUG_PRINT("cmp"+kind.name()+"ss", dst, src);  write0x0F(0xF3, 0xC2, dst, src, kind.code()); }

    inline void cmpss(CmpCode kind, Xmm reg, Mem128 mem)
        { R64FX_JIT_DEBUG_PRINT("cmp"+kind.name()+"ss", reg, mem);  write0x0F(0xF3, 0xC2, reg, mem, kind.code()); }

    inline void cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp)
        { R64FX_JIT_DEBUG_PRINT("cmp"+kind.name()+"ss", reg, base, disp);  write0x0F(0xF3, 0xC2, reg, base, disp, kind.code()); }

    inline void cmpltps(Xmm dst, Xmm src) { cmpps(LT, dst, src); }
    inline void cmpltps(Xmm reg, Mem128 mem) { cmpps(LT, reg, mem); }

    inline void cmpnltps(Xmm dst, Xmm src) { cmpps(NLT, dst, src); }
    inline void cmpnltps(Xmm reg, Mem128 mem) { cmpps(NLT, reg, mem); }

    inline void cmpeqps(Xmm dst, Xmm src) { cmpps(EQ, dst, src); }
    inline void cmpeqps(Xmm reg, Mem128 mem) { cmpps(EQ, reg, mem); }

    inline void movss(Xmm reg, Mem32 mem)
        { R64FX_JIT_DEBUG_PRINT("movss", reg, mem);  write0x0F(0xF3, 0x10, reg, mem); }

    inline void movss(Mem32 mem, Xmm reg)
        { R64FX_JIT_DEBUG_PRINT("movss", mem, reg);  write0x0F(0xF3, 0x11, reg, mem); }

    inline void shufps(Xmm dst, Xmm src, Shuf shuf)
        { R64FX_JIT_DEBUG_PRINT("shufps", dst, src, shuf);  write0x0F(0, 0xC6, dst, src, shuf.byte()); }

    inline void shufps(Xmm reg, Mem128 mem, Shuf shuf)
        { R64FX_JIT_DEBUG_PRINT("shufps", reg, mem, shuf);  write0x0F(0, 0xC6, reg, mem, shuf.byte()); }

    inline void pshufd(Xmm dst, Xmm src, Shuf shuf)
        { R64FX_JIT_DEBUG_PRINT("pshufd", dst, src, shuf);  write0x0F(0x66, 0x70, dst, src, shuf.byte()); }

    inline void pshufd(Xmm reg, Mem128 mem, Shuf shuf)
        { R64FX_JIT_DEBUG_PRINT("pshufd", reg, mem, shuf);  write0x0F(0x66, 0x70, reg, mem, shuf.byte()); }

    inline void pshufd(Xmm reg, Base base, Disp8 disp, Shuf shuf)
        { R64FX_JIT_DEBUG_PRINT("pshufd", reg, base, disp, shuf);  write0x0F(0x66, 0x70, reg, base, disp, shuf.byte()); }

    inline void cvtps2dq(Xmm dst, Xmm src)
        { R64FX_JIT_DEBUG_PRINT("cvtps2dq", dst, src);  write0x0F(0x66, 0x5B, dst, src); }

    inline void cvtps2dq(Xmm reg, Mem128 mem)
        { R64FX_JIT_DEBUG_PRINT("cvtps2dq", reg, mem);  write0x0F(0x66, 0x5B, reg, mem); }

    inline void cvtps2dq(Xmm reg, Base base, Disp8 disp)
        { R64FX_JIT_DEBUG_PRINT("cvtps2dq", reg, base, disp);  write0x0F(0x66, 0x5B, reg, base, disp); }

    inline void cvtdq2ps(Xmm dst, Xmm src)
        { R64FX_JIT_DEBUG_PRINT("cvtdq2ps", dst, src);  write0x0F(0, 0x5B, dst, src); }

    inline void cvtdq2ps(Xmm reg, Mem128 mem)
        { R64FX_JIT_DEBUG_PRINT("cvtdq2ps", reg, mem);  write0x0F(0, 0x5B, reg, mem); }

    inline void cvtdq2ps(Xmm reg, Base base, Disp8 disp)
        { R64FX_JIT_DEBUG_PRINT("cvtdq2ps", reg, base, disp);  write0x0F(0, 0x5B, reg, base, disp); }

    inline void paddd(Xmm dst, Xmm src)
        { R64FX_JIT_DEBUG_PRINT("paddd", dst, src);  write0x0F(0x66, 0xFE, dst, src);}

    inline void paddd(Xmm reg, Mem128 mem)
        { R64FX_JIT_DEBUG_PRINT("paddd", reg, mem); write0x0F(0x66, 0xFE, reg, mem); }

    inline void paddd(Xmm reg, Base base, Disp8 disp)
        { R64FX_JIT_DEBUG_PRINT("paddd", reg, base, disp); write0x0F(0x66, 0xFE, reg, base, disp); }

    inline void psubd(Xmm dst, Xmm src)
        { R64FX_JIT_DEBUG_PRINT("psubd", dst, src);  write0x0F(0x66, 0xFA, dst, src); }

    inline void psubd(Xmm reg, Mem128 mem)
        { R64FX_JIT_DEBUG_PRINT("psubd", reg, mem);  write0x0F(0x66, 0xFA, reg, mem); }

    inline void psubd(Xmm reg, Base base, Disp8 disp)
        { R64FX_JIT_DEBUG_PRINT("psubd", reg, base, disp);  write0x0F(0x66, 0xFA, reg, base, disp); }

#ifdef R64FX_JIT_DEBUG_STDOUT
private:
    inline void printIp() { std::cout << (void*)ip(); }

    inline void printName(const std::string &name)
    {
        std::cout << "    " << name;
        int i = name.size();
        while(i < 10) { std::cout << ' '; i++; }
    }

    inline void print(const std::string &name)
    {
        printIp(); printName(name); std::cout << "\n";
    }

    inline void print(const std::string &name, int count)
    {
        printIp(); printName(name); std::cout << count << "\n";
    }

    template<typename RegT>
    inline void print(const std::string &name, RegT reg)
    {
        printIp(); printName(name);
        std::cout << reg.name() << "\n";
    }

    inline void print(const std::string &name, Mem8 mem)
    {
        printIp(); printName(name);
        std::cout << "[" << (void*)mem.addr() << "]\n";
    }

    inline void print(const std::string &name, GPR64 reg, Imm32 imm)
    {
        printIp(); printName(name);
        std::cout << reg.name() << ", " << imm.s << "\n";
    }

    inline void print(const std::string &name, GPR64 reg, Imm64 imm)
    {
        printIp(); printName(name);
        std::cout << reg.name() << ", " << imm.s << "\n";
    }

    template<typename RegT>
    inline void print(const std::string &name, RegT dst, RegT src)
    {
        printIp(); printName(name);
        std::cout << dst.name() << ", " << src.name() << "\n";
    }

    inline void print(const std::string &name, Xmm dst, Xmm src, Shuf shuf)
    {
        printIp(); printName(name);
        std::cout << dst.name() << ", " << src.name() << ", " << shuf.str() << "\n";
    }

    template<typename RegT>
    inline void print(const std::string &name, RegT reg, Mem8 mem)
    {
        printIp(); printName(name);
        std::cout << reg.name() << ", [" << (void*)mem.addr() << "]\n";
    }

    inline void print(const std::string &name, Xmm reg, Mem8 mem, Shuf shuf)
    {
        printIp(); printName(name);
        std::cout << reg.name() << ", [" << (void*)mem.addr() << "], " << shuf.str() << "\n";
    }

    template<typename RegT>
    inline void print(const std::string &name, Mem8 mem, RegT reg)
    {
        printIp(); printName(name);
        std::cout << "[" << (void*)mem.addr() << "], " << reg.name() << "\n";
    }

    template<typename RegT>
    inline void print(const std::string &name, RegT reg, Base base, Disp8 disp)
    {
        printIp(); printName(name);
        std::cout << reg.name() << ", [" << base.reg.name() << "]";
        if(disp.byte) std::cout << " + " << int(disp.byte);
        std::cout << "\n";
    }

    inline void print(const std::string &name, Xmm reg, Base base, Disp8 disp, Shuf shuf)
    {
        printIp(); printName(name);
        std::cout << reg.name() << ", [" << base.reg.name() << "]";
        if(disp.byte) std::cout << " + " << int(disp.byte);
        std::cout << shuf.str() << "\n";
    }

    template<typename RegT>
    inline void print(const std::string &name, Base base, Disp8 disp, RegT reg )
    {
        printIp(); printName(name);
        std::cout << ", [" << base.reg.name() << "]";
        if(disp.byte) std::cout << " + " << int(disp.byte);
        std::cout << ", " << reg.name() << "\n";
    }
#endif//R64FX_JIT_DEBUG_STDOUT
};//Assembler

}//namespace r64fx


#endif//X86_64_JIT_ASSEMBLER_H
