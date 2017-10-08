#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#include "Debug.hpp"
#include "MemoryUtils.hpp"
#include "binary_constants.hpp"

namespace r64fx{

union Imm8{
    unsigned char c;
    explicit Imm8(unsigned char c) : c(c) {}
};

union Imm16{
    unsigned short n;
    unsigned char  b[2];
    explicit Imm16(unsigned short n) : n(n) {}
};

union Imm32{
    unsigned int   n;
    unsigned char  b[4];
    explicit Imm32(unsigned int n) : n(n) {}
};

union Imm64{
    unsigned long  n;
    unsigned char  b[8];
    explicit Imm64(unsigned long n) : n(n) {}
};

inline Imm64 ImmAddr(void* addr)
{
    return Imm64((unsigned long)addr);
}


class Register{
    unsigned char m_bits;

protected:
    Register(unsigned char bits) : m_bits(bits) {}

public:
    inline unsigned int bits() const { return m_bits; }

    inline unsigned int lowerBits() const { return m_bits & 7; }

    /* R or B bit of the REX prefix.*/
    inline bool prefix_bit() const { return m_bits & 8; }
};

class GPR64 : public Register{
public:
    GPR64(unsigned char code) : Register(code) {}
};

#define rax GPR64(0x0)
#define rcx GPR64(0x1)
#define rdx GPR64(0x2)
#define rbx GPR64(0x3)
#define rsb GPR64(0x4)
#define rbp GPR64(0x5)
#define rsi GPR64(0x6)
#define rdi GPR64(0x7)
#define r8  GPR64(0x8)
#define r9  GPR64(0x9)
#define r10 GPR64(0xA)
#define r11 GPR64(0xB)
#define r12 GPR64(0xC)
#define r13 GPR64(0xD)
#define r14 GPR64(0xE)
#define r15 GPR64(0xF)


class Xmm : public Register{
public:
    Xmm(unsigned char code) : Register(code) {}
};

#define xmm0  Xmm(0x0)
#define xmm1  Xmm(0x1)
#define xmm2  Xmm(0x2)
#define xmm3  Xmm(0x3)
#define xmm4  Xmm(0x4)
#define xmm5  Xmm(0x5)
#define xmm6  Xmm(0x6)
#define xmm7  Xmm(0x7)
#define xmm8  Xmm(0x8)
#define xmm9  Xmm(0x9)
#define xmm10 Xmm(0xA)
#define xmm11 Xmm(0xB)
#define xmm12 Xmm(0xC)
#define xmm13 Xmm(0xD)
#define xmm14 Xmm(0xE)
#define xmm15 Xmm(0xF)


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


class Base{
    unsigned char m_bits = 0;

public:
    explicit Base(GPR64 reg) : m_bits(reg.bits()) {}

    unsigned int bits() const { return m_bits; }
};

class Index{
    unsigned long m_bits = 0;

    inline static unsigned long encodeScale(int scale)
    {
#ifdef R64FX_DEBUG
        assert(scale == 1 || scale == 2 || scale == 4 || scale == 8);
#endif//R64FX_DEBUG
        if(scale == 1)
            return 0;
        if(scale == 2)
            return 1 << 8;
        if(scale == 4)
            return 2 << 8;
        return 3 << 8;
    }

public:
    explicit Index(GPR64 reg, int scale) : m_bits(0x400 | encodeScale(scale) | (reg.bits() << 4))
    {
#ifdef R64FX_DEBUG
        assert(reg.bits() != 4);
#endif//R64FX_DEBUG
    }

    inline unsigned long bits() const { return m_bits; }
};

class Disp{
    int m_disp = 0;

public:
    explicit Disp(int disp) : m_disp(disp) {}

    inline int value() const { return m_disp; };
};

class SIB{
    friend SIB operator+(Base base, Index index);

    unsigned long m_bits = 0;

protected:
    SIB(unsigned long bits) : m_bits(bits) {}

public:
    inline unsigned long bits() const { return m_bits; }

    inline GPR64 base() const { return GPR64(m_bits & 0xF); }

    inline GPR64 index() const { return GPR64((m_bits & 0xF0) >> 4); }

    inline unsigned char scale() const { return (m_bits & 0x300) >> 8; }

    inline bool hasIndex() const { return m_bits & 0x400; }
};

class SIBD : public SIB{
    friend class Assembler;
    friend SIBD operator+(SIB sib, Disp disp);
    friend SIBD operator+(Base base, Disp disp);

    SIBD(unsigned long bits) : SIB(bits) {}

    unsigned char modrmByte(const Register &reg) const;

    unsigned char sibByte() const;

    int dispBytes() const;

public:
    SIBD(Base base) : SIB(base.bits()) {}

    SIBD(SIB sib) : SIB(sib.bits()) {}

    inline int disp() const { return (bits() >> 32) & 0xFFFFFFFF; }
};

inline SIB operator+(Base base, Index index) { return SIB(base.bits() | index.bits()); }

inline SIBD operator+(Base base, Disp disp) { return SIBD(base.bits() | long(disp.value()) << 32); }

inline SIBD operator+(SIB sib, Disp disp) { return SIBD(sib.bits() | long(disp.value()) << 32); }


class CmpCode{
    unsigned int m_code;

public:
    CmpCode(unsigned int code) : m_code(code) {}

    inline unsigned int code() const { return m_code; }
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


class JumpLabel{
    friend class Assembler;

    int m_jmp_addr = 0;
    int m_imm_addr = 0;

    inline void setJmpAddr(int offset) { m_jmp_addr = offset; }

    inline void setImmAddr(int offset) { m_imm_addr = offset; }

    inline int jmpAddr() const { return m_jmp_addr; }

    inline int immAddr() const { return m_imm_addr; }

public:
    JumpLabel() {};

    ~JumpLabel() { if(immAddr() != 0) { R64FX_DEBUG_ASSERT(jmpAddr() != 0); } }
};


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
};


class Assembler{
    unsigned char*  m_buffer          = nullptr;

    //Decreases with data allocated.
    unsigned char*  m_data_begin      = nullptr;

    //Fixed. Points to a location between code and data.
    unsigned char*  m_code_begin      = nullptr;

    //Increases with code written.
    unsigned char*  m_code_end        = nullptr;

    unsigned char*  m_buffer_end      = nullptr;

public:
    Assembler() {}

    ~Assembler() { resize(0, 0); }

    void resize(unsigned long data_page_count, unsigned long code_page_count);


    inline unsigned char* dataBegin() const { return m_data_begin; }

    inline void setDataBegin(unsigned char* addr)
    {
        R64FX_DEBUG_ASSERT(addr >= m_buffer);
        R64FX_DEBUG_ASSERT(addr <= m_code_begin);
        m_data_begin = addr;
    }

    inline unsigned char* dataEnd() const { return m_code_begin; }


    inline unsigned char* codeBegin() const { return m_code_begin; }

    inline unsigned char* codeEnd() const { return m_code_end; }

    inline void setCodeEnd(unsigned char* addr)
    {
        R64FX_DEBUG_ASSERT(addr >= m_code_begin);
        R64FX_DEBUG_ASSERT(addr <= m_buffer_end);
        m_code_end = addr;
    }

    inline void rewindCode() { setCodeEnd(codeBegin()); }


    inline unsigned long dataBufferSize() const { return m_code_begin - m_buffer; }

    inline unsigned long dataBytesUsed() const { return m_code_begin - m_data_begin; }

    inline unsigned long dataBytesAvailable() const { return m_data_begin - m_buffer; }


    inline unsigned long codeBufferSize() const { return m_buffer_end - m_code_begin; }

    inline unsigned long codeBytesUsed() const { return m_code_end - m_code_begin; }

    inline unsigned long codeBytesAvailable() const { return m_buffer_end - m_code_end; }


    inline unsigned long dataPageCount() const
    {
        R64FX_DEBUG_ASSERT((dataBufferSize() % memory_page_size()) == 0);
        return dataBufferSize() / memory_page_size();
    }

    inline unsigned long codePageCount() const
    {
        R64FX_DEBUG_ASSERT((codeBufferSize() % memory_page_size()) == 0);
        return codeBufferSize() / memory_page_size();
    }

    /* Grow data buffer. Resize if needed.
     * Returns an offset value. Subtract it from codeBegin().
     */
    int growData(int nbytes);

private:
    /* Ensure that code buffer has room to add nbytes. Resize if needed. */
    unsigned char* growCode(int nbytes);

    void fill(unsigned char byte, int nbytes);

    void write(unsigned char byte);
    void write(unsigned char byte0, unsigned char byte1);

    void write(unsigned char opcode, unsigned char r, GPR64 reg, Imm32 imm);
    void write(unsigned char opcode, GPR64 reg, Imm64 imm);

    void write(unsigned char opcode, GPR64 dst, GPR64 src);
    void write(unsigned char opcode, GPR64 reg, Mem64 mem);
    void write(unsigned char opcode, GPR64 reg, SIBD sibd);

    void write(unsigned char opcode, Mem8 mem);
    void write(unsigned char opcode1, unsigned char opcode2, Mem8 mem);

    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm dst, Xmm src, int imm = -1);
    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, Mem8 mem, int imm = -1);
    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, SIBD sibd, int imm = -1);

    void write(unsigned char opcode, GPR64 reg);

    void write(unsigned char opcode1, unsigned char opcode2, JumpLabel &label);

public:
    inline void nop(int count) { write(0x90, count); }
    inline void ret()          { write(0xC3); }
    inline void rdtsc()        { write(0x0F, 0x31); }

    inline void mov(GPR64 reg, Imm32 imm){ write(0xC7, 0, reg, imm); }
    inline void mov(GPR64 reg, Imm64 imm){ write(0xB8, reg, imm); }
    inline void mov(GPR64 dst, GPR64 src){ write(0x8B, dst, src); }
    inline void mov(GPR64 reg, Mem64 mem){ write(0x8B, reg, mem); }
    inline void mov(Mem64 mem, GPR64 reg){ write(0x89, reg, mem); }
    inline void mov(GPR64 reg, SIBD sibd){ write(0x8B, reg, sibd); }
    inline void mov(SIBD sibd, GPR64 reg){ write(0x89, reg, sibd); }

    inline void add(GPR64 reg, Imm32 imm){ write(0x81, 0, reg, imm); }
    inline void add(GPR64 dst, GPR64 src){ write(0x03, dst, src); }
    inline void add(GPR64 reg, Mem64 mem){ write(0x03, reg, mem); }
    inline void add(Mem64 mem, GPR64 reg){ write(0x01, reg, mem); }
    inline void add(GPR64 reg, SIBD sibd){ write(0x03, reg, sibd); }
    inline void add(SIBD sibd, GPR64 reg){ write(0x01, reg, sibd); }

    inline void sub(GPR64 reg, Imm32 imm){ write(0x81, 5, reg, imm); }
    inline void sub(GPR64 dst, GPR64 src){ write(0x2B, dst, src); }
    inline void sub(GPR64 reg, Mem64 mem){ write(0x2B, reg, mem); }
    inline void sub(Mem64 mem, GPR64 reg){ write(0x29, reg, mem); }
    inline void sub(GPR64 reg, SIBD sibd){ write(0x2B, reg, sibd); }
    inline void sub(SIBD sibd, GPR64 reg){ write(0x29, reg, sibd); }

    inline void push (GPR64 reg){ write(0x50, reg); }
    inline void pop  (GPR64 reg){ write(0x58, reg); }

    inline void cmp(GPR64 reg, Imm32 imm){ write(0x81, 7, reg, imm); }

    void mark(JumpLabel &label);

    inline void jmp (JumpLabel &label){ write(0,    0xE9, label); }
    inline void jnz (JumpLabel &label){ write(0x0F, 0x85, label); }
    inline void jz  (JumpLabel &label){ write(0x0F, 0x84, label); }
    inline void je  (JumpLabel &label){ write(0x0F, 0x84, label); }
    inline void jne (JumpLabel &label){ write(0x0F, 0x85, label); }
    inline void jl  (JumpLabel &label){ write(0x0F, 0x8C, label); }

/* === SSE === */

    inline void movaps(Xmm dst, Xmm src)    { write0x0F(0, 0x28, dst, src); }
    inline void movaps(Xmm reg, Mem128 mem) { write0x0F(0, 0x28, reg, mem); }
    inline void movaps(Mem128 mem, Xmm reg) { write0x0F(0, 0x29, reg, mem); }
    inline void movaps(Xmm reg, SIBD sibd)  { write0x0F(0, 0x28, reg, sibd); }
    inline void movaps(SIBD sibd, Xmm reg)  { write0x0F(0, 0x29, reg, sibd); }

    inline void movups(Xmm dst, Xmm src)   { write0x0F(0, 0x10, dst, src); }
    inline void movups(Xmm reg, Mem32 mem) { write0x0F(0, 0x10, reg, mem); }
    inline void movups(Mem32 mem, Xmm reg) { write0x0F(0, 0x11, reg, mem); }
    inline void movups(Xmm reg, SIBD sibd) { write0x0F(0, 0x10, reg, sibd); }
    inline void movups(SIBD sibd, Xmm reg) { write0x0F(0, 0x11, reg, sibd); }

private:
    inline void sse_ps_instruction(unsigned char second_opcode_byte, Xmm dst, Xmm src)
        { write0x0F(0, second_opcode_byte, dst, src); }

    inline void sse_ps_instruction(unsigned char second_opcode_byte, Xmm reg, Mem128 mem)
        { write0x0F(0, second_opcode_byte, reg, mem); }

    inline void sse_ps_instruction(unsigned char second_opcode_byte, Xmm reg, SIBD sibd)
        { write0x0F(0, second_opcode_byte, reg, sibd); }

    inline void sse_ss_instruction(unsigned char third_opcode_byte, Xmm dst, Xmm src)
        { write0x0F(0xF3, third_opcode_byte, dst, src); }

    inline void sse_ss_instruction(unsigned char third_opcode_byte, Xmm reg, Mem32 mem)
        { write0x0F(0xF3, third_opcode_byte, reg, mem); }

    inline void sse_ss_instruction(unsigned char third_opcode_byte, Xmm reg, SIBD sibd)
        { write0x0F(0xF3, third_opcode_byte, reg, sibd); }

#define ENCODE_SSE_PS_INSTRUCTION(name, second_opcode_byte)\
inline void name(Xmm dst, Xmm src)    { sse_ps_instruction(second_opcode_byte, dst, src); }\
inline void name(Xmm reg, Mem128 mem) { sse_ps_instruction(second_opcode_byte, reg, mem); }\
inline void name(Xmm reg, SIBD sibd)  { sse_ps_instruction(second_opcode_byte, reg, sibd); }

#define ENCODE_SSE_SS_INSTRUCTION(name, third_opcode_byte)\
inline void name(Xmm dst, Xmm src)   { sse_ss_instruction(third_opcode_byte, dst, src); }\
inline void name(Xmm reg, Mem32 mem) { sse_ss_instruction(third_opcode_byte, reg, mem); }\
inline void name(Xmm reg, SIBD sibd) { sse_ss_instruction(third_opcode_byte, reg, sibd); }

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

#undef ENCODE_SSE_INSTRUCTION
#undef ENCODE_SSE_PS_INSTRUCTION
#undef ENCODE_SSE_SS_INSTRUCTION

    inline void cmpps(CmpCode kind, Xmm dst, Xmm src)    { write0x0F(0, 0xC2, dst, src,  kind.code()); }
    inline void cmpps(CmpCode kind, Xmm reg, Mem128 mem) { write0x0F(0, 0xC2, reg, mem,  kind.code()); }
    inline void cmpps(CmpCode kind, Xmm reg, SIBD sibd)  { write0x0F(0, 0xC2, reg, sibd, kind.code()); }

    inline void cmpss(CmpCode kind, Xmm dst, Xmm src)    { write0x0F(0xF3, 0xC2, dst, src,  kind.code()); }
    inline void cmpss(CmpCode kind, Xmm reg, Mem128 mem) { write0x0F(0xF3, 0xC2, reg, mem,  kind.code()); }
    inline void cmpss(CmpCode kind, Xmm reg, SIBD sibd)  { write0x0F(0xF3, 0xC2, reg, sibd, kind.code()); }

    inline void cmpltps(Xmm dst, Xmm src)    { cmpps(LT, dst, src); }
    inline void cmpltps(Xmm reg, Mem128 mem) { cmpps(LT, reg, mem); }

    inline void cmpnltps(Xmm dst, Xmm src)    { cmpps(NLT, dst, src); }
    inline void cmpnltps(Xmm reg, Mem128 mem) { cmpps(NLT, reg, mem); }

    inline void cmpeqps(Xmm dst, Xmm src)    { cmpps(EQ, dst, src); }
    inline void cmpeqps(Xmm reg, Mem128 mem) { cmpps(EQ, reg, mem); }

    inline void movss(Xmm reg, Mem32 mem) { write0x0F(0xF3, 0x10, reg, mem); }
    inline void movss(Mem32 mem, Xmm reg) { write0x0F(0xF3, 0x11, reg, mem); }

    inline void shufps(Xmm dst, Xmm src,    Shuf shuf) { write0x0F(0, 0xC6, dst, src,  shuf.byte()); }
    inline void shufps(Xmm reg, Mem128 mem, Shuf shuf) { write0x0F(0, 0xC6, reg, mem,  shuf.byte()); }
    inline void shufps(Xmm reg, SIBD sibd,  Shuf shuf) { write0x0F(0, 0xC6, reg, sibd, shuf.byte()); }

    inline void pshufd(Xmm dst, Xmm src,    Shuf shuf) { write0x0F(0x66, 0x70, dst, src,  shuf.byte()); }
    inline void pshufd(Xmm reg, Mem128 mem, Shuf shuf) { write0x0F(0x66, 0x70, reg, mem,  shuf.byte()); }
    inline void pshufd(Xmm reg, SIBD sibd,  Shuf shuf) { write0x0F(0x66, 0x70, reg, sibd, shuf.byte()); }

    inline void cvtps2dq(Xmm dst, Xmm src)    { write0x0F(0x66, 0x5B, dst, src); }
    inline void cvtps2dq(Xmm reg, Mem128 mem) { write0x0F(0x66, 0x5B, reg, mem); }
    inline void cvtps2dq(Xmm reg, SIBD sibd)  { write0x0F(0x66, 0x5B, reg, sibd); }

    inline void cvtdq2ps(Xmm dst, Xmm src)    { write0x0F(0, 0x5B, dst, src); }
    inline void cvtdq2ps(Xmm reg, Mem128 mem) { write0x0F(0, 0x5B, reg, mem); }
    inline void cvtdq2ps(Xmm reg, SIBD sibd)  { write0x0F(0, 0x5B, reg, sibd); }

/* === SSE2 === */

    inline void paddd(Xmm dst, Xmm src)    { write0x0F(0x66, 0xFE, dst, src);}
    inline void paddd(Xmm reg, Mem128 mem) { write0x0F(0x66, 0xFE, reg, mem); }
    inline void paddd(Xmm reg, SIBD sibd)  { write0x0F(0x66, 0xFE, reg, sibd); }

    inline void psubd(Xmm dst, Xmm src)    { write0x0F(0x66, 0xFA, dst, src); }
    inline void psubd(Xmm reg, Mem128 mem) { write0x0F(0x66, 0xFA, reg, mem); }
    inline void psubd(Xmm reg, SIBD sibd)  { write0x0F(0x66, 0xFA, reg, sibd); }

};//Assembler

}//namespace r64fx


#endif//X86_64_JIT_ASSEMBLER_H
