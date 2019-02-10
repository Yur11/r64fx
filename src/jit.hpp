#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#include "Debug.hpp"
#include "MemoryUtils.hpp"

namespace r64fx{

class Register{
    unsigned char m_bits = 0;

public:
    explicit Register(unsigned char bits) : m_bits(bits) {}

    unsigned char bits() { return m_bits; }

    unsigned int code() { return m_bits & 0xF; }

    unsigned int lowerBits() { return m_bits & 0x7; }

    bool prefixBit() { return m_bits & 0x8; }

    bool rexW() { return bits() & 0x80; }

    bool rex() { return rexW() || prefixBit(); }
};

class GPR : public Register{
public:
    explicit GPR(unsigned char code = 0) : Register(code) {}
};

class GPR32 : public GPR{
public:
    explicit GPR32(unsigned char code = 0) : GPR(code) {}

    static unsigned int Size() { return 4; }
};
inline GPR32 eax(0x0), ecx(0x1), edx (0x2), ebx (0x3), esp (0x4), ebp (0x5), esi (0x6), edi (0x7),
             r8d(0x8), r9d(0x9), r10d(0xA), e11d(0xB), r12d(0xC), r13d(0xD), r14d(0xE), r15d(0xF);

class GPR64 : public GPR{
public:
    explicit GPR64(unsigned char code = 0) : GPR(code | 0x80) {}

    GPR32 low32() { return GPR32(code()); }

    static unsigned int Size() { return 8; }
};
inline GPR64 rax(0x0), rcx(0x1), rdx(0x2), rbx(0x3), rsp(0x4), rbp(0x5), rsi(0x6), rdi(0x7),
             r8 (0x8), r9 (0x9), r10(0xA), r11(0xB), r12(0xC), r13(0xD), r14(0xE), r15(0xF);

class Mmx : public Register{};

class Xmm : public Register{
public:
    explicit Xmm(unsigned char code = 0) : Register(code) {}

    static unsigned int Size() { return 16; }
};
inline Xmm xmm0(0x0), xmm1(0x1), xmm2 (0x2), xmm3 (0x3), xmm4 (0x4), xmm5 (0x5), xmm6 (0x6), xmm7 (0x7),
           xmm8(0x8), xmm9(0x9), xmm10(0xA), xmm11(0xB), xmm12(0xC), xmm13(0xD), xmm14(0xE), xmm15(0xF);

class Ymm : public Register{
public:
    explicit Ymm(unsigned char code = 0) : Register(code) {}

    static unsigned int Size() { return 32; }
};
inline Ymm ymm0(0x0), ymm1(0x1), ymm2 (0x2), ymm3 (0x3), ymm4 (0x4), ymm5 (0x5), ymm6 (0x6), ymm7 (0x7),
           ymm8(0x8), ymm9(0x9), ymm10(0xA), ymm11(0xB), ymm12(0xC), ymm13(0xD), ymm14(0xE), ymm15(0xF);

template<typename RegT> inline bool operator==(RegT a, RegT b) { return a.code() == b.code(); }
template<typename RegT> inline bool operator!=(RegT a, RegT b) { return a.code() != b.code(); }

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
    float          f;
    unsigned char  b[4];
    explicit Imm32(unsigned int n) : n(n) {}
};

inline Imm32 Imm32f(float f) { Imm32 imm(0); imm.f = f; return imm; }

union Imm64{
    unsigned long  n;
    double         d;
    float          f[2];
    unsigned char  b[8];
    explicit Imm64(unsigned long n) : n(n) {}
};

inline Imm64 Imm64f(float f0, float f1 = 0.0f) { Imm64 imm(0); imm.f[0] = f0; imm.f[1] = f1; return imm; }

inline Imm64 Imm64D(double d) { Imm64 imm(0); imm.d = d; return imm; }

inline Imm64 ImmAddr(void* addr) { return Imm64((unsigned long)addr); }




#ifdef R64FX_DEBUG
#define R64FX_JIT_DEBUG_MEM_ALIGN(bytes) assert((long int)addr % bytes == 0);
#else
#define R64FX_JIT_DEBUG_MEM_ALIGN(bytes)
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


struct MemArg{
    unsigned int bits = 0;
    unsigned int disp = 0;

    MemArg() {}
    explicit MemArg(unsigned int bits) : bits(bits) {}

    inline void setBase(GPR64 reg)   { bits |= (0x400 | reg.code()); }
    inline bool hasBase() const      { return bits & 0x400; }
    inline GPR64 base() const        { return GPR64(bits & 0xF);  }

    inline void setIndex(GPR64 reg)  { bits |= (0x800 | (reg.code() << 4)); }
    inline bool hasIndex() const     { return bits & 0x800; }
    inline GPR64 index() const       { return GPR64((bits >> 4) & 0xF);  }

    inline void setScale(unsigned int scale)
    {
        R64FX_DEBUG_ASSERT(scale == 1 || scale == 2 || scale == 4 || scale == 8);
        if(scale == 1) scale = 0; else
        if(scale == 2) scale = 1; else
        if(scale == 4) scale = 2; else
        if(scale == 8) scale = 3;
        bits |= scale << 8;
    }
    inline unsigned int scaleBits() const { return (bits >> 8) & 3; }

    inline void setDisplacement(int val)  { disp = val; bits |= (0x1000); }
    inline bool hasDisplacement() const   { return bits & 0x1000; }
    inline int  displacement() const      { return disp; }
};


struct Base : public MemArg{
    explicit Base(GPR64 reg) { setBase(reg); }
};

struct Index : public MemArg{
    explicit Index(GPR64 reg) { R64FX_DEBUG_ASSERT(reg != rsp); setIndex(reg); }
};

struct ScaledIndex : public MemArg{
    explicit ScaledIndex(Index index, unsigned int scale) : MemArg(index.bits) { setScale(scale); }
};

inline ScaledIndex operator*(Index index, unsigned int scale) { return ScaledIndex(index, scale); }

struct SIB : public MemArg{
    SIB(Base base) : MemArg(base.bits) {}

    explicit SIB(Base base,       Index index) : MemArg(base.bits | index.bits) {}
    explicit SIB(Base base, ScaledIndex index) : MemArg(base.bits | index.bits) {}
};

inline SIB operator+(Base base,       Index index) { return SIB(base, index); }
inline SIB operator+(Base base, ScaledIndex index) { return SIB(base, index); }

struct Disp{ int disp = 0;
    explicit Disp(int disp) : disp(disp) {}
};

struct SIBD : public MemArg{
    SIBD(Base base) : MemArg(base.bits) {}
    SIBD(SIB   sib) : MemArg(sib.bits) {}
    explicit SIBD(SIB sib, Disp disp) : MemArg(sib.bits) { setDisplacement(disp.disp); }
    explicit SIBD(Index index, Disp disp) : MemArg(index.bits) { setDisplacement(disp.disp); }
    explicit SIBD(ScaledIndex index, Disp disp) : MemArg(index.bits) { setDisplacement(disp.disp); }
};

template<typename A, typename B> inline SIBD operator+(A a, B b) { return SIBD(a, b); }


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
    friend class AssemblerBuffer;

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


#define R64FX_JIT_LABEL(label) r64fx::JumpLabel label; mark(label);


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

    Shuf(unsigned char byte) : m_byte(byte) {}

    inline unsigned char byte() const { return m_byte; }

    inline int s0() const { return m_byte & 3; }
    inline int s1() const { return (m_byte >> 2) & 3; }
    inline int s2() const { return (m_byte >> 4) & 3; }
    inline int s3() const { return (m_byte >> 6) & 3; }
};


class Opcode{
    unsigned long m = 0;

public:
    explicit Opcode(unsigned long code) : m(code) { R64FX_DEBUG_ASSERT((code & ~0x7FF) == 0); }

    inline unsigned char code() const { return m & 0xFF; }

    inline bool has0F() const { return m & 0x100; }

    inline bool has66() const { return m & 0x200; }

    inline bool hasF3() const { return m & 0x400; }

    inline int byteCount() const { return 1 + has0F() + has66() + hasF3(); }
};

inline Opcode Opcode_0F   (unsigned long code) { return Opcode(0x100 | code); }
inline Opcode Opcode_660F (unsigned long code) { return Opcode(0x300 | code); }
inline Opcode Opcode_F30F (unsigned long code) { return Opcode(0x500 | code); }


struct Operands{
    unsigned long m = 0;

    explicit Operands(unsigned char r);

    explicit Operands(unsigned char r, Register rm);

    explicit Operands(unsigned char r, Register rm, Imm8 imm);

    explicit Operands(unsigned char r, Register rm, Imm32 imm);

    explicit Operands(Register r, Register rm, int imm = 0x100);

    explicit Operands(Register r, Mem8 mem, unsigned char* rip, int imm = 0x100);

    explicit Operands(Register r, SIBD sibd, int imm = 0x100);

    inline int operandByteCount() const { return m & 0xF; } //Excluding REX

    inline unsigned long wrxb() const { return (m >> 4) & 0xF; }

    inline bool hasRex() const { return wrxb(); }

    inline int totalByteCount() const //Including REX
    {
        return operandByteCount() + hasRex();
    }

    inline unsigned long operandBytes() const { return m >> 8; }
};


class AssemblerBuffer : public MemoryBuffer{
    AssemblerBuffer(const AssemblerBuffer &other) {} //No Copy!

public:
    AssemblerBuffer() {}

    ~AssemblerBuffer() {}

    void permitExecution();

    void prohibitExecution();

    void fill(unsigned char byte, int nbytes);

    void write(unsigned char byte);
    void write(unsigned char byte0, unsigned char byte1);

    void write(unsigned char opcode, Register reg, Imm64 imm);
    void write(unsigned char opcode, Register reg);

    void write(unsigned char opcode1, unsigned char opcode2, JumpLabel &label);

    void write(const Opcode &opcode, const Operands &operands);

    void markLabel(JumpLabel &label);

    unsigned long cpu_features = 0;
};


/* AssemblerInstructions mixin class.
   Make sure that AssemblerBaseT containes AssemblerBase member called m. */
template<class AssemblerBaseT> class AssemblerInstructions : public AssemblerBaseT{
public:
    using AssemblerBaseT::AssemblerBaseT;
    using AssemblerBaseT::m;

    inline unsigned char* IP() { return m.ptr(); }

    inline void nopAlign(unsigned long align)
    {
        unsigned long extra_bytes = ((unsigned long)m.ptr()) % align;
        if(extra_bytes)
            NOP(align - extra_bytes);
    }

    inline void NOP()          { m.write(0x90); }
    inline void NOP(int count) { m.fill(0x90, count); }
    inline void RET()          { m.write(0xC3); }
    inline void RDTSC()        { m.write(0x0F, 0x31); }
    inline void CPUID()        { m.write(0x0F, 0xA2); }
    inline void LFENCE()       { m.write(Opcode_0F(0xAE), Operands(5)); }
    inline void MFENCE()       { m.write(Opcode_0F(0xAE), Operands(6)); }
    inline void SFENCE()       { m.write(Opcode_0F(0xAE), Operands(7)); }

    inline void LEA(GPR64 reg, Mem8 mem)  { m.write(Opcode(0x8D), Operands(reg, mem, m.ptr() + 6)); }
    inline void LEA(GPR64 reg, SIBD sibd) { m.write(Opcode(0x8D), Operands(reg, sibd)); }

    inline void leaNextInstruction(GPR64 reg) { LEA(reg, Mem8(m.ptr() + 7)); }

    inline void MOV(GPR32 reg, Imm32 imm){ m.write(Opcode(0xC7), Operands(0, reg, imm)); }
    inline void MOV(GPR64 reg, Imm32 imm){ m.write(Opcode(0xC7), Operands(0, reg, imm)); }
    inline void MOV(GPR64 reg, Imm64 imm){ m.write(0xB8, reg, imm); }

    inline void MOV(GPR64 dst, GPR64 src){ m.write(Opcode(0x8B), Operands(dst, src)); }
    inline void MOV(GPR32 reg, Mem32 mem){ m.write(Opcode(0x8B), Operands(reg, mem, m.ptr() + 6)); }
    inline void MOV(GPR64 reg, Mem64 mem){ m.write(Opcode(0x8B), Operands(reg, mem, m.ptr() + 6)); }

    inline void MOV(Mem32 mem, GPR32 reg){ m.write(Opcode(0x89), Operands(reg, mem, m.ptr() + 6)); }
    inline void MOV(Mem64 mem, GPR64 reg){ m.write(Opcode(0x89), Operands(reg, mem, m.ptr() + 6)); }

    inline void MOV(GPR32 reg, SIBD sibd){ m.write(Opcode(0x8B), Operands(reg, sibd)); }
    inline void MOV(GPR64 reg, SIBD sibd){ m.write(Opcode(0x8B), Operands(reg, sibd)); }
    inline void MOV(SIBD sibd, GPR32 reg){ m.write(Opcode(0x89), Operands(reg, sibd)); }
    inline void MOV(SIBD sibd, GPR64 reg){ m.write(Opcode(0x89), Operands(reg, sibd)); }

    inline void CMOVZ(GPR64 dst, GPR64 src) { m.write(Opcode_0F(0x44), Operands(dst, src)); }

#define R64FX_GPR_INST(name, rrm, r)\
    inline void name(GPR64 reg, Imm8  imm){ m.write(Opcode(0x83),     Operands(r,  reg, imm)); }\
    inline void name(GPR64 reg, Imm32 imm){ m.write(Opcode(0x81),     Operands(r,  reg, imm)); }\
    inline void name(GPR64 dst, GPR64 src){ m.write(Opcode(rrm + 2),  Operands(dst, src)); }\
    inline void name(GPR64 reg, Mem64 mem){ m.write(Opcode(rrm + 2),  Operands(reg, mem, m.ptr() + 6)); }\
    inline void name(Mem64 mem, GPR64 reg){ m.write(Opcode(rrm),      Operands(reg, mem, m.ptr() + 6)); }\
    inline void name(GPR64 reg, SIBD sibd){ m.write(Opcode(rrm + 2),  Operands(reg, sibd)); }\
    inline void name(SIBD sibd, GPR64 reg){ m.write(Opcode(rrm),      Operands(reg, sibd)); }\
    inline void name(GPR32 reg, SIBD sibd){ m.write(Opcode(rrm + 2),  Operands(reg, sibd)); }\
    inline void name(SIBD sibd, GPR32 reg){ m.write(Opcode(rrm),      Operands(reg, sibd)); }\

    R64FX_GPR_INST(ADD,  0x01, 0)
    R64FX_GPR_INST(SUB,  0x29, 5)
    R64FX_GPR_INST(XOR,  0x31, 6)
    R64FX_GPR_INST(AND,  0x21, 4)
    R64FX_GPR_INST(OR,   0x09, 1)

#undef R64FX_GPR_INST

#define R64FX_GPR_SHIFT_INST(name, r)\
    inline void name(GPR32 gpr) { m.write(Opcode(0xD1), Operands(r, gpr)); }\
    inline void name(GPR64 gpr) { m.write(Opcode(0xD1), Operands(r, gpr)); }\
    inline void name(GPR32 gpr, Imm8 imm) { m.write(Opcode(0xC1), Operands(r, gpr, imm)); }\
    inline void name(GPR64 gpr, Imm8 imm) { m.write(Opcode(0xC1), Operands(r, gpr, imm)); }

    R64FX_GPR_SHIFT_INST(SHL, 4)
    R64FX_GPR_SHIFT_INST(SHR, 5)
    R64FX_GPR_SHIFT_INST(SAR, 7)

#undef R64FX_GPR_SHIFT_INST

    inline void PUSH (GPR64 reg){ m.write(0x50, reg); }
    inline void POP  (GPR64 reg){ m.write(0x58, reg); }

    inline void mark(JumpLabel &label) { m.markLabel(label); }

    inline void CMP(GPR64 reg, Imm32 imm){ m.write(0x81, 7, reg, imm); }

    inline void JMP(GPR64 reg) { m.write(Opcode(0xFF), Operands(4, reg)); }

    inline void JMP (JumpLabel &label){ m.write(0,    0xE9, label); }
    inline void JNZ (JumpLabel &label){ m.write(0x0F, 0x85, label); }
    inline void JZ  (JumpLabel &label){ m.write(0x0F, 0x84, label); }
    inline void JE  (JumpLabel &label){ m.write(0x0F, 0x84, label); }
    inline void JNE (JumpLabel &label){ m.write(0x0F, 0x85, label); }
    inline void JL  (JumpLabel &label){ m.write(0x0F, 0x8C, label); }


/* === SSE === */

    inline void MOVAPS(Xmm dst, Xmm src)    { m.write(Opcode_0F(0x28), Operands(dst, src)); }
    inline void MOVAPS(Xmm reg, Mem128 mem) { m.write(Opcode_0F(0x28), Operands(reg, mem, m.ptr() + 7)); }
    inline void MOVAPS(Mem128 mem, Xmm reg) { m.write(Opcode_0F(0x29), Operands(reg, mem, m.ptr() + 7)); }
    inline void MOVAPS(Xmm reg, SIBD sibd)  { m.write(Opcode_0F(0x28), Operands(reg, sibd)); }
    inline void MOVAPS(SIBD sibd, Xmm reg)  { m.write(Opcode_0F(0x29), Operands(reg, sibd)); }

    inline void MOVUPS(Xmm dst, Xmm src)   { m.write(Opcode_0F(0x10), Operands(dst, src)); }
    inline void MOVUPS(Xmm reg, Mem32 mem) { m.write(Opcode_0F(0x10), Operands(reg, mem, m.ptr() + 7)); }
    inline void MOVUPS(Mem32 mem, Xmm reg) { m.write(Opcode_0F(0x11), Operands(reg, mem, m.ptr() + 7)); }
    inline void MOVUPS(Xmm reg, SIBD sibd) { m.write(Opcode_0F(0x10), Operands(reg, sibd)); }
    inline void MOVUPS(SIBD sibd, Xmm reg) { m.write(Opcode_0F(0x11), Operands(reg, sibd)); }

    inline void MOVLPS(Xmm reg, Mem64 mem) { m.write(Opcode_0F(0x12), Operands(reg, mem, m.ptr() + 7)); }
    inline void MOVLPS(Xmm reg, SIBD sibd) { m.write(Opcode_0F(0x12), Operands(reg, sibd)); }
    inline void MOVLPS(Mem64 mem, Xmm reg) { m.write(Opcode_0F(0x13), Operands(reg, mem, m.ptr() + 7));}
    inline void MOVLPS(SIBD sibd, Xmm reg) { m.write(Opcode_0F(0x13), Operands(reg, sibd)); }

    inline void MOVHPS(Xmm reg, Mem64 mem) { m.write(Opcode_0F(0x16), Operands(reg, mem, m.ptr() + 7)); }
    inline void MOVHPS(Xmm reg, SIBD sibd) { m.write(Opcode_0F(0x16), Operands(reg, sibd)); }
    inline void MOVHPS(Mem64 mem, Xmm reg) { m.write(Opcode_0F(0x17), Operands(reg, mem, m.ptr() + 7));}
    inline void MOVHPS(SIBD sibd, Xmm reg) { m.write(Opcode_0F(0x17), Operands(reg, sibd)); }

    inline void MOVHLPS(Xmm dst, Xmm src)  { m.write(Opcode_0F(0x12), Operands(dst, src)); }
    inline void MOVLHPS(Xmm dst, Xmm src)  { m.write(Opcode_0F(0x16), Operands(dst, src)); }

private:
    inline void sse_ps_instruction(unsigned char opcode, Xmm dst, Xmm src)
        { m.write(Opcode_0F(opcode), Operands(dst, src)); }

    inline void sse_ps_instruction(unsigned char opcode, Xmm reg, Mem128 mem)
        { m.write(Opcode_0F(opcode), Operands(reg, mem, m.ptr() + 7)); }

    inline void sse_ps_instruction(unsigned char opcode, Xmm reg, SIBD sibd)
        { m.write(Opcode_0F(opcode), Operands(reg, sibd)); }

    inline void sse_ss_instruction(unsigned char opcode, Xmm dst, Xmm src)
        { m.write(Opcode_F30F(opcode), Operands(dst, src)); }

    inline void sse_ss_instruction(unsigned char opcode, Xmm reg, Mem32 mem)
        { m.write(Opcode_F30F(opcode), Operands(reg, mem, m.ptr() + 8)); }

    inline void sse_ss_instruction(unsigned char opcode, Xmm reg, SIBD sibd)
        { m.write(Opcode_F30F(opcode), Operands(reg, sibd)); }

#define R64FX_SSE_PS_INSTRUCTION(name, opcode)\
inline void name(Xmm dst, Xmm src)    { sse_ps_instruction(opcode, dst, src); }\
inline void name(Xmm reg, Mem128 mem) { sse_ps_instruction(opcode, reg, mem); }\
inline void name(Xmm reg, SIBD sibd)  { sse_ps_instruction(opcode, reg, sibd); }

#define R64FX_SSE_SS_INSTRUCTION(name, opcode)\
inline void name(Xmm dst, Xmm src)   { sse_ss_instruction(opcode, dst, src); }\
inline void name(Xmm reg, Mem32 mem) { sse_ss_instruction(opcode, reg, mem); }\
inline void name(Xmm reg, SIBD sibd) { sse_ss_instruction(opcode, reg, sibd); }

#define R64FX_SSE_INSTRUCTION(name, opcode)\
    R64FX_SSE_PS_INSTRUCTION(name##PS, opcode)\
    R64FX_SSE_SS_INSTRUCTION(name##SS, opcode)

public:
    R64FX_SSE_INSTRUCTION    (ADD,   0x58)
    R64FX_SSE_INSTRUCTION    (SUB,   0x5C)
    R64FX_SSE_INSTRUCTION    (MUL,   0x59)
    R64FX_SSE_INSTRUCTION    (DIV,   0x5E)
    R64FX_SSE_INSTRUCTION    (RCP,   0x53)
    R64FX_SSE_INSTRUCTION    (SQRT,  0x51)
    R64FX_SSE_INSTRUCTION    (RSQRT, 0x52)
    R64FX_SSE_INSTRUCTION    (MAX,   0x5F)
    R64FX_SSE_INSTRUCTION    (MIN,   0x5D)

    R64FX_SSE_PS_INSTRUCTION (ANDPS,   0x54)
    R64FX_SSE_PS_INSTRUCTION (ANDNPS,  0x55)
    R64FX_SSE_PS_INSTRUCTION (ORPS,    0x56)
    R64FX_SSE_PS_INSTRUCTION (XORPS,   0x57)

#undef R64FX_SSE_INSTRUCTION
#undef R64FX_SSE_PS_INSTRUCTION
#undef R64FX_SSE_SS_INSTRUCTION

    inline void CMPPS(CmpCode kind, Xmm dst, Xmm src)    { m.write(Opcode_0F(0xC2), Operands(dst, src,  kind.code())); }
    inline void CMPPS(CmpCode kind, Xmm reg, Mem128 mem) { m.write(Opcode_0F(0xC2), Operands(reg, mem,  kind.code())); }
    inline void CMPPS(CmpCode kind, Xmm reg, SIBD sibd)  { m.write(Opcode_0F(0xC2), Operands(reg, sibd, kind.code())); }

    inline void CMPSS(CmpCode kind, Xmm dst, Xmm src)    { m.write(Opcode_F30F(0xC2), Operands(dst, src,  kind.code())); }
    inline void CMPSS(CmpCode kind, Xmm reg, Mem128 mem) { m.write(Opcode_F30F(0xC2), Operands(reg, mem,  kind.code())); }
    inline void CMPSS(CmpCode kind, Xmm reg, SIBD sibd)  { m.write(Opcode_F30F(0xC2), Operands(reg, sibd, kind.code())); }

    inline void CMPLTPS(Xmm dst, Xmm src)    { CMPPS(LT, dst, src); }
    inline void CMPLTPS(Xmm reg, Mem128 mem) { CMPPS(LT, reg, mem); }

    inline void CMPNLTPS(Xmm dst, Xmm src)    { CMPPS(NLT, dst, src); }
    inline void CMPNLTPS(Xmm reg, Mem128 mem) { CMPPS(NLT, reg, mem); }

    inline void CMPEQPS(Xmm dst, Xmm src)    { CMPPS(EQ, dst, src); }
    inline void CMPEQPS(Xmm reg, Mem128 mem) { CMPPS(EQ, reg, mem); }

    inline void MOVSS(Xmm reg, Mem32 mem) { m.write(Opcode_F30F(0x10), Operands(reg, mem, m.ptr() + 8)); }
    inline void MOVSS(Mem32 mem, Xmm reg) { m.write(Opcode_F30F(0x11), Operands(reg, mem, m.ptr() + 8)); }
    inline void MOVSS(Xmm reg, SIBD sibd) { m.write(Opcode_F30F(0x10), Operands(reg, sibd)); }

    inline void SHUFPS(Xmm dst, Xmm src,    Shuf shuf) { m.write(Opcode_0F(0xC6), Operands(dst, src,  shuf.byte())); }
    inline void SHUFPS(Xmm reg, Mem128 mem, Shuf shuf) { m.write(Opcode_0F(0xC6), Operands(reg, mem, m.ptr() + 8, shuf.byte())); }
    inline void SHUFPS(Xmm reg, SIBD sibd,  Shuf shuf) { m.write(Opcode_0F(0xC6), Operands(reg, sibd, shuf.byte())); }

    inline void CVTPS2DQ(Xmm dst, Xmm src)    { m.write(Opcode_660F(0x5B), Operands(dst, src)); }
    inline void CVTPS2DQ(Xmm reg, Mem128 mem) { m.write(Opcode_660F(0x5B), Operands(reg, mem, m.ptr() + 8)); }
    inline void CVTPS2DQ(Xmm reg, SIBD sibd)  { m.write(Opcode_660F(0x5B), Operands(reg, sibd)); }

    inline void CVTDQ2PS(Xmm dst, Xmm src)    { m.write(Opcode_0F(0x5B), Operands(dst, src)); }
    inline void CVTDQ2PS(Xmm reg, Mem128 mem) { m.write(Opcode_0F(0x5B), Operands(reg, mem, m.ptr() + 7)); }
    inline void CVTDQ2PS(Xmm reg, SIBD sibd)  { m.write(Opcode_0F(0x5B), Operands(reg, sibd)); }


/* === SSE2 === */
    inline void MOVDQA(Xmm dst, Xmm src)    { m.write(Opcode_660F(0x6F), Operands(dst, src)); }
    inline void MOVDQA(Xmm dst, Mem128 mem) { m.write(Opcode_660F(0x6F), Operands(dst, mem, m.ptr() + 8)); }
    inline void MOVDQA(Xmm dst, SIBD sibd)  { m.write(Opcode_660F(0x6F), Operands(dst, sibd)); }
    inline void MOVDQA(Mem128 mem, Xmm src) { m.write(Opcode_660F(0x7F), Operands(src, mem, m.ptr() + 8)); }
    inline void MOVDQA(SIBD sibd, Xmm src)  { m.write(Opcode_660F(0x7F), Operands(src, sibd)); }

    inline void MOVD(Xmm dst, GPR32 src) { m.write(Opcode_660F(0x6E), Operands(dst, src)); }
    inline void MOVD(Xmm dst, Mem32 mem) { m.write(Opcode_660F(0x6E), Operands(dst, mem, m.ptr() + 8)); }
    inline void MOVD(Xmm dst, SIBD sibd) { m.write(Opcode_660F(0x6E), Operands(dst, sibd)); }

    inline void MOVQ(Xmm dst, GPR64 src) { m.write(Opcode_660F(0x6E), Operands(dst, src)); }

    inline void MOVD(GPR32 dst, Xmm src) { m.write(Opcode_660F(0x7E), Operands(src, dst)); }
    inline void MOVD(Mem32 mem, Xmm src) { m.write(Opcode_660F(0x7E), Operands(src, mem, m.ptr() + 8)); }
    inline void MOVD(SIBD sibd, Xmm src) { m.write(Opcode_660F(0x7E), Operands(src, sibd)); }

    inline void MOVQ(GPR64 dst, Xmm src) { m.write(Opcode_660F(0x7E), Operands(src, dst)); }
    inline void MOVD(Xmm dst, GPR64 src) { MOVD(dst, src.low32()); }
    inline void MOVD(GPR64 dst, Xmm src) { MOVD(dst.low32(), src); }

    inline void PSHUFD(Xmm dst, Xmm src,    Shuf shuf) { m.write(Opcode_660F(0x70), Operands(dst, src,  shuf.byte())); }
    inline void PSHUFD(Xmm reg, Mem128 mem, Shuf shuf) { m.write(Opcode_660F(0x70), Operands(reg, mem, m.ptr() + 9, shuf.byte())); }
    inline void PSHUFD(Xmm reg, SIBD sibd,  Shuf shuf) { m.write(Opcode_660F(0x70), Operands(reg, sibd, shuf.byte())); }

#define R64FX_SSE2_INSTRUCTION(name, opcode)\
    inline void name(Xmm dst, Xmm src)    { m.write(Opcode_660F(opcode), Operands(dst, src));}\
    inline void name(Xmm reg, Mem128 mem) { m.write(Opcode_660F(opcode), Operands(reg, mem, m.ptr() + 8)); }\
    inline void name(Xmm reg, SIBD sibd)  { m.write(Opcode_660F(opcode), Operands(reg, sibd)); }

    R64FX_SSE2_INSTRUCTION(PADDD,      0xFE)
    R64FX_SSE2_INSTRUCTION(PSUBD,      0xFA)
    R64FX_SSE2_INSTRUCTION(PADDQ,      0xD4)
    R64FX_SSE2_INSTRUCTION(PSUBQ,      0xFB)
    R64FX_SSE2_INSTRUCTION(PAND,       0xDB)
    R64FX_SSE2_INSTRUCTION(PXOR,       0xEF)
    R64FX_SSE2_INSTRUCTION(POR,        0xEB)
    R64FX_SSE2_INSTRUCTION(PCMPEQB,    0x74)
    R64FX_SSE2_INSTRUCTION(PCMPEQW,    0x75)
    R64FX_SSE2_INSTRUCTION(PCMPEQD,    0x76)
    R64FX_SSE2_INSTRUCTION(PCMPGTB,    0x64)
    R64FX_SSE2_INSTRUCTION(PCMPGTW,    0x65)
    R64FX_SSE2_INSTRUCTION(PCMPGTD,    0x66)
    R64FX_SSE2_INSTRUCTION(PUNPCKLBW,  0x60)
    R64FX_SSE2_INSTRUCTION(PUNPCKHBW,  0x68)
    R64FX_SSE2_INSTRUCTION(PUNPCKLWD,  0x61)
    R64FX_SSE2_INSTRUCTION(PUNPCKHWD,  0x69)
    R64FX_SSE2_INSTRUCTION(PUNPCKLDQ,  0x62)
    R64FX_SSE2_INSTRUCTION(PUNPCKHDQ,  0x6A)

    inline void PSRLW(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x71), Operands(2, reg, imm)); }
    inline void PSRLD(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x72), Operands(2, reg, imm)); }
    inline void PSRLQ(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x73), Operands(2, reg, imm)); }
    inline void PSRAW(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x71), Operands(4, reg, imm)); }
    inline void PSRAD(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x72), Operands(4, reg, imm)); }
    inline void PSLLW(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x71), Operands(6, reg, imm)); }
    inline void PSLLD(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x72), Operands(6, reg, imm)); }
    inline void PSLLQ(Xmm reg, Imm8 imm) { m.write(Opcode_660F(0x73), Operands(6, reg, imm)); }
};


class AssemblerBase : public AssemblerBuffer{
protected:
    AssemblerBuffer &m;
public:
    AssemblerBase() : m(*this) {}
};

typedef AssemblerInstructions<AssemblerBase> Assembler;


struct JitCpuFeatures{
    unsigned int m_ecx_bits1;
    unsigned int m_edx_bits1;
    unsigned int m_ebx_bits7;

    inline bool hasSSE   () const { return m_edx_bits1 & (1<<25); }
    inline bool hasSSE2  () const { return m_edx_bits1 & (1<<26); }

    inline bool hasSSE3  () const { return m_ecx_bits1 & 1; }
    inline bool hasSSSE3 () const { return m_ecx_bits1 & (1<<9); }
    inline bool hasSSE41 () const { return m_ecx_bits1 & (1<<19); }
    inline bool hasSSE42 () const { return m_ecx_bits1 & (1<<20); }
    inline bool hasAVX   () const { return m_ecx_bits1 & (1<<28); }
    inline bool hasAVX2  () const { return m_ebx_bits7 & (1<<5); }
    inline bool hasFMA   () const { return m_ecx_bits1 & (1<<12); }
};


template<typename AssemblerT> JitCpuFeatures get_cpu_features(AssemblerT* as)
{
    JitCpuFeatures features;

    bool must_free = false;
    if(!as->m.ptr())
    {
        as->resize(0, 1);
        must_free = true;
    }
    auto ptr = as->m.ptr();

    as->PUSH  (rbx);

    as->MOV   (rax, Imm32(1));
    as->CPUID ();
    as->MOV   (Base(rdi), ecx);
    as->MOV   (Base(rdi) + Disp(4), edx);

    as->MOV   (rax, Imm32(7));
    as->MOV   (rcx, Imm32(0));
    as->CPUID ();
    as->MOV   (Base(rdi) + Disp(8), ebx);

    as->POP   (rbx);
    as->RET   ();

    ((void(*)(void*))ptr)(&features);

    as->setCodeEnd(ptr);

    if(must_free)
    {
        as->resize(0, 0);
    }
    return features;
}


template<typename StreamT> StreamT &operator<<(StreamT &stream, const JitCpuFeatures &cpu)
{
    stream << "SSE:    " << cpu.hasSSE() << "\n";
    stream << "SSE2:   " << cpu.hasSSE2() << "\n";
    stream << "SSE3:   " << cpu.hasSSE3() << "\n";
    stream << "SSSE3:  " << cpu.hasSSE3() << "\n";
    stream << "SSE4.1: " << cpu.hasSSE41() << "\n";
    stream << "SSE4.2: " << cpu.hasSSE42() << "\n";
    stream << "AVX:    " << cpu.hasAVX() << "\n";
    stream << "AVX2:   " << cpu.hasAVX2() << "\n";
    stream << "FMA:    " << cpu.hasFMA() << "\n";
    return stream;
}

}//namespace r64fx


#endif//X86_64_JIT_ASSEMBLER_H
