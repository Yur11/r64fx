#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#include "Debug.hpp"
#include "MemoryUtils.hpp"

namespace r64fx{

class Register{
    unsigned char m_bits;

protected:
    constexpr Register(unsigned char bits) : m_bits(bits) {}

    constexpr unsigned char bits() { return m_bits; }

public:

    constexpr unsigned int code() { return m_bits & 0xF; }

    constexpr unsigned int lowerBits() { return m_bits & 0x7; }

    constexpr bool prefixBit() { return m_bits & 0x8; }
};

class GPR : public Register{
public:
    explicit constexpr GPR(unsigned char code) : Register(code) {}

    constexpr bool is64bit() { return bits() & 0x80; }

    constexpr bool isCalleePreserved()
        { return (1<<code()) & ((1<<0x3) | (1<<0x5) | (1<<0xC) | (1<<0xD) | (1<<0xE) | (1<<0xF)); }
};

class GPR32 : public GPR{
public:
    explicit constexpr GPR32(unsigned char code) : GPR(code) {}
};
constexpr GPR32 eax(0x0), ecx(0x1), edx (0x2), ebx (0x3), esp (0x4), ebp (0x5), esi (0x6), edi (0x7),
                r8d(0x8), r9d(0x9), r10d(0xA), e11d(0xB), r12d(0xC), r13d(0xD), r14d(0xE), r15d(0xF);

class GPR64 : public GPR{
public:
    explicit constexpr GPR64(unsigned char code) : GPR(code | 0x80) {}

    constexpr GPR32 gpr32() { return GPR32(code()); }
};
constexpr GPR64 rax(0x0), rcx(0x1), rdx(0x2), rbx(0x3), rsp(0x4), rbp(0x5), rsi(0x6), rdi(0x7),
                r8 (0x8), r9 (0x9), r10(0xA), r11(0xB), r12(0xC), r13(0xD), r14(0xE), r15(0xF);

class Xmm : public Register{
public:
    explicit constexpr Xmm(unsigned char code) : Register(code) {}
};
constexpr Xmm xmm0(0x0), xmm1(0x1), xmm2 (0x2), xmm3 (0x3), xmm4 (0x4), xmm5 (0x5), xmm6 (0x6), xmm7 (0x7),
              xmm8(0x8), xmm9(0x9), xmm10(0xA), xmm11(0xB), xmm12(0xC), xmm13(0xD), xmm14(0xE), xmm15(0xF);


union Imm8{
    unsigned char b;
    explicit Imm8(unsigned char b) : b(b) {}
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
    explicit Base(GPR64 reg) : m_bits(reg.code()) {}

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
    explicit Index(GPR64 reg, int scale) : m_bits(0x400 | encodeScale(scale) | (reg.code() << 4))
    {
#ifdef R64FX_DEBUG
        assert(reg.code() != rsp.code());
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

    inline bool rexW() const
    {
        return base().is64bit() || (hasIndex() && index().is64bit());
    }

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


/*  Pack four 0..3 values into a sigle byte. To be used with shuffle instructions.

    Each parameter defines the source scalar from which to read the data into the current position.
    Setting s3 to 0, means moving the first scalar form the source vector into the last scalar of the destination vector.
        - Shuf(0, 1, 2, 3) - no shuffle.
        - Shuf(3, 2, 1, 0) - reverse order.
 */
class Shuf{
    unsigned char m_byte = 0;

public:
    Shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3)
    {
        m_byte = (s3 << 6) + (s2 << 4) + (s1 << 2) + s0;
    }

    inline unsigned char byte() const { return m_byte; }

    inline int s0() const { return m_byte & 3; }
    inline int s1() const { return (m_byte >> 2) & 3; }
    inline int s2() const { return (m_byte >> 4) & 3; }
    inline int s3() const { return (m_byte >> 6) & 3; }
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

    inline void rewindData() { setDataBegin(dataEnd()); }

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
     * Returns an offset value. Subtract it from codeBegin() to get the pointer.
     */
    long growData(int nbytes);

private:
    /* Ensure that code buffer has room to add nbytes. Resize if needed. */
    unsigned char* growCode(int nbytes);

    void fill(unsigned char byte, int nbytes);

    void write(unsigned char byte);
    void write(unsigned char byte0, unsigned char byte1);

    void write(unsigned char opcode, unsigned char r, GPR reg);
    void write(unsigned char opcode, unsigned char r, GPR reg, Imm8  imm);
    void write(unsigned char opcode, unsigned char r, GPR reg, Imm32 imm);
    void write(unsigned char opcode, GPR64 reg, Imm64 imm);

    void write(unsigned char opcode, GPR64 dst, GPR64 src);
    void write(unsigned char opcode, GPR   reg, Mem32 mem);
    void write(unsigned char opcode, GPR64 reg, SIBD sibd);

    void write(unsigned char opcode, Mem8 mem);
    void write(unsigned char opcode1, unsigned char opcode2, Mem8 mem);

    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm dst, Xmm src, int imm = -1);
    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, Mem8 mem, int imm = -1);
    void write0x0F(unsigned char pre_rex_byte, unsigned byte1, Xmm reg, SIBD sibd, int imm = -1);

    void write(unsigned char opcode, GPR64 reg);

    void write(unsigned char opcode1, unsigned char opcode2, JumpLabel &label);

public:
    inline void NOP()          { write(0x90); }
    inline void NOP(int count) { fill(0x90, count); }
    inline void RET()          { write(0xC3); }
    inline void RDTSC()        { write(0x0F, 0x31); }

    inline void MOV(GPR32 reg, Imm32 imm){ write(0xC7, 0, reg, imm); }
    inline void MOV(GPR64 reg, Imm32 imm){ write(0xC7, 0, reg, imm); }
    inline void MOV(GPR64 reg, Imm64 imm){ write(0xB8, reg, imm); }

    inline void MOV(GPR64 dst, GPR64 src){ write(0x8B, dst, src); }
    inline void MOV(GPR32 reg, Mem32 mem){ write(0x8B, reg, mem); }
    inline void MOV(GPR64 reg, Mem64 mem){ write(0x8B, reg, mem); }

    inline void MOV(Mem32 mem, GPR32 reg){ write(0x89, reg, mem); }
    inline void MOV(Mem64 mem, GPR64 reg){ write(0x89, reg, mem); }

//     inline void mov(GPR32 reg, SIBD sibd){ write(0x8B, reg, sibd); }
    inline void MOV(GPR64 reg, SIBD sibd){ write(0x8B, reg, sibd); }
//     inline void mov(SIBD sibd, GPR32 reg){ write(0x89, reg, sibd); }
    inline void MOV(SIBD sibd, GPR64 reg){ write(0x89, reg, sibd); }

#define R64FX_GPR_INST(name, rrm, r)\
    inline void name(GPR64 reg, Imm8  imm){ write(0x83, r,  reg, imm); }\
    inline void name(GPR64 reg, Imm32 imm){ write(0x81, r,  reg, imm); }\
    inline void name(GPR64 dst, GPR64 src){ write(rrm + 2,  dst, src); }\
    inline void name(GPR64 reg, Mem64 mem){ write(rrm + 2,  reg, mem); }\
    inline void name(Mem64 mem, GPR64 reg){ write(rrm,      reg, mem); }\
    inline void name(GPR64 reg, SIBD sibd){ write(rrm + 2,  reg, sibd); }\
    inline void name(SIBD sibd, GPR64 reg){ write(rrm, reg, sibd); }\

    R64FX_GPR_INST(ADD,  0x01, 0)
    R64FX_GPR_INST(SUB,  0x29, 5)
    R64FX_GPR_INST(XOR, 0x31, 6)
    R64FX_GPR_INST(AND, 0x21, 4)
    R64FX_GPR_INST(OR,  0x09, 1)

#undef R64FX_GPR_INST

#define R64FX_GPR_SHIFT_INST(name, r)\
    inline void name(GPR32 gpr) { write(0xD1, r, gpr); }\
    inline void name(GPR64 gpr) { write(0xD1, r, gpr); }\
    inline void name(GPR32 gpr, Imm8 imm) { write(0xC1, r, gpr, imm); }\
    inline void name(GPR64 gpr, Imm8 imm) { write(0xC1, r, gpr, imm); }

    R64FX_GPR_SHIFT_INST(SHL, 4)
    R64FX_GPR_SHIFT_INST(SHR, 5)
    R64FX_GPR_SHIFT_INST(SAR, 7)

#undef R64FX_GPR_SHIFT_INST

    inline void PUSH (GPR64 reg){ write(0x50, reg); }
    inline void POP  (GPR64 reg){ write(0x58, reg); }

    inline void CMP(GPR64 reg, Imm32 imm){ write(0x81, 7, reg, imm); }

    void mark(JumpLabel &label);

    inline void JMP (JumpLabel &label){ write(0,    0xE9, label); }
    inline void JNZ (JumpLabel &label){ write(0x0F, 0x85, label); }
    inline void JZ  (JumpLabel &label){ write(0x0F, 0x84, label); }
    inline void JE  (JumpLabel &label){ write(0x0F, 0x84, label); }
    inline void JNE (JumpLabel &label){ write(0x0F, 0x85, label); }
    inline void JL  (JumpLabel &label){ write(0x0F, 0x8C, label); }

/* === SSE === */

    inline void MOVAPS(Xmm dst, Xmm src)    { write0x0F(0, 0x28, dst, src); }
    inline void MOVAPS(Xmm reg, Mem128 mem) { write0x0F(0, 0x28, reg, mem); }
    inline void MOVAPS(Mem128 mem, Xmm reg) { write0x0F(0, 0x29, reg, mem); }
    inline void MOVAPS(Xmm reg, SIBD sibd)  { write0x0F(0, 0x28, reg, sibd); }
    inline void MOVAPS(SIBD sibd, Xmm reg)  { write0x0F(0, 0x29, reg, sibd); }

    inline void MOVUPS(Xmm dst, Xmm src)   { write0x0F(0, 0x10, dst, src); }
    inline void MOVUPS(Xmm reg, Mem32 mem) { write0x0F(0, 0x10, reg, mem); }
    inline void MOVUPS(Mem32 mem, Xmm reg) { write0x0F(0, 0x11, reg, mem); }
    inline void MOVUPS(Xmm reg, SIBD sibd) { write0x0F(0, 0x10, reg, sibd); }
    inline void MOVUPS(SIBD sibd, Xmm reg) { write0x0F(0, 0x11, reg, sibd); }

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
    ENCODE_SSE_PS_INSTRUCTION(name##PS, opcode)\
    ENCODE_SSE_SS_INSTRUCTION(name##SS, opcode)

public:
    ENCODE_SSE_INSTRUCTION(ADD,   0x58)
    ENCODE_SSE_INSTRUCTION(SUB,   0x5C)
    ENCODE_SSE_INSTRUCTION(MUL,   0x59)
    ENCODE_SSE_INSTRUCTION(DIV,   0x5E)
    ENCODE_SSE_INSTRUCTION(RCP,   0x53)
    ENCODE_SSE_INSTRUCTION(SQRT,  0x51)
    ENCODE_SSE_INSTRUCTION(RSQRT, 0x52)
    ENCODE_SSE_INSTRUCTION(MAX,   0x5F)
    ENCODE_SSE_INSTRUCTION(MIN,   0x5D)
    ENCODE_SSE_PS_INSTRUCTION(ANDPS,   0x54)
    ENCODE_SSE_PS_INSTRUCTION(ANDNPS,  0x55)
    ENCODE_SSE_PS_INSTRUCTION(ORPS,    0x56)
    ENCODE_SSE_PS_INSTRUCTION(XORPS,   0x57)

#undef ENCODE_SSE_INSTRUCTION
#undef ENCODE_SSE_PS_INSTRUCTION
#undef ENCODE_SSE_SS_INSTRUCTION

    inline void CMPPS(CmpCode kind, Xmm dst, Xmm src)    { write0x0F(0, 0xC2, dst, src,  kind.code()); }
    inline void CMPPS(CmpCode kind, Xmm reg, Mem128 mem) { write0x0F(0, 0xC2, reg, mem,  kind.code()); }
    inline void CMPPS(CmpCode kind, Xmm reg, SIBD sibd)  { write0x0F(0, 0xC2, reg, sibd, kind.code()); }

    inline void CMPSS(CmpCode kind, Xmm dst, Xmm src)    { write0x0F(0xF3, 0xC2, dst, src,  kind.code()); }
    inline void CMPSS(CmpCode kind, Xmm reg, Mem128 mem) { write0x0F(0xF3, 0xC2, reg, mem,  kind.code()); }
    inline void CMPSS(CmpCode kind, Xmm reg, SIBD sibd)  { write0x0F(0xF3, 0xC2, reg, sibd, kind.code()); }

    inline void CMPLTPS(Xmm dst, Xmm src)    { CMPPS(LT, dst, src); }
    inline void CMPLTPS(Xmm reg, Mem128 mem) { CMPPS(LT, reg, mem); }

    inline void CMPNLTPS(Xmm dst, Xmm src)    { CMPPS(NLT, dst, src); }
    inline void CMPNLTPS(Xmm reg, Mem128 mem) { CMPPS(NLT, reg, mem); }

    inline void CMPEQPS(Xmm dst, Xmm src)    { CMPPS(EQ, dst, src); }
    inline void CMPEQPS(Xmm reg, Mem128 mem) { CMPPS(EQ, reg, mem); }

    inline void MOVSS(Xmm reg, Mem32 mem) { write0x0F(0xF3, 0x10, reg, mem); }
    inline void MOVSS(Mem32 mem, Xmm reg) { write0x0F(0xF3, 0x11, reg, mem); }

    inline void SHUFPS(Xmm dst, Xmm src,    Shuf shuf) { write0x0F(0, 0xC6, dst, src,  shuf.byte()); }
    inline void SHUFPS(Xmm reg, Mem128 mem, Shuf shuf) { write0x0F(0, 0xC6, reg, mem,  shuf.byte()); }
    inline void SHUFPS(Xmm reg, SIBD sibd,  Shuf shuf) { write0x0F(0, 0xC6, reg, sibd, shuf.byte()); }

    inline void PSHUFD(Xmm dst, Xmm src,    Shuf shuf) { write0x0F(0x66, 0x70, dst, src,  shuf.byte()); }
    inline void PSHUFD(Xmm reg, Mem128 mem, Shuf shuf) { write0x0F(0x66, 0x70, reg, mem,  shuf.byte()); }
    inline void PSHUFD(Xmm reg, SIBD sibd,  Shuf shuf) { write0x0F(0x66, 0x70, reg, sibd, shuf.byte()); }

    inline void CVTPS2DQ(Xmm dst, Xmm src)    { write0x0F(0x66, 0x5B, dst, src); }
    inline void CVTPS2DQ(Xmm reg, Mem128 mem) { write0x0F(0x66, 0x5B, reg, mem); }
    inline void CVTPS2DQ(Xmm reg, SIBD sibd)  { write0x0F(0x66, 0x5B, reg, sibd); }

    inline void CVTDQ2PS(Xmm dst, Xmm src)    { write0x0F(0, 0x5B, dst, src); }
    inline void CVTDQ2PS(Xmm reg, Mem128 mem) { write0x0F(0, 0x5B, reg, mem); }
    inline void CVTDQ2PS(Xmm reg, SIBD sibd)  { write0x0F(0, 0x5B, reg, sibd); }

/* === SSE2 === */

    inline void PADDD(Xmm dst, Xmm src)    { write0x0F(0x66, 0xFE, dst, src);}
    inline void PADDD(Xmm reg, Mem128 mem) { write0x0F(0x66, 0xFE, reg, mem); }
    inline void PADDD(Xmm reg, SIBD sibd)  { write0x0F(0x66, 0xFE, reg, sibd); }

    inline void PSUBD(Xmm dst, Xmm src)    { write0x0F(0x66, 0xFA, dst, src); }
    inline void PSUBD(Xmm reg, Mem128 mem) { write0x0F(0x66, 0xFA, reg, mem); }
    inline void PSUBD(Xmm reg, SIBD sibd)  { write0x0F(0x66, 0xFA, reg, sibd); }

};//Assembler

}//namespace r64fx


#endif//X86_64_JIT_ASSEMBLER_H
