#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#ifdef R64FX_DEBUG_JIT_STDOUT
#include <iostream>
#endif//R64FX_DEBUG_JIT_STDOUT_JIT_STDOUT

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
#ifdef R64FX_DEBUG_JIT_STDOUT
    static const char* names[];
#endif//R64FX_DEBUG_JIT_STDOUT

public:
    explicit GPR64(const unsigned char bits) : Register(bits) {}

#ifdef R64FX_DEBUG_JIT_STDOUT
    inline const char* name() const { return names[Register::code()]; }
#endif//R64FX_DEBUG_JIT_STDOUT
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
#ifdef R64FX_DEBUG_JIT_STDOUT
    static const char* names[];
#endif//R64FX_DEBUG_JIT_STDOUT

public:
    explicit Xmm(const unsigned char bits) : Register(bits) {}

#ifdef R64FX_DEBUG_JIT_STDOUT
    inline const char* name() const { return names[Register::code()]; }
#endif//R64FX_DEBUG_JIT_STDOUT
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


struct Mem8{
    long int addr;

    Mem8(void* addr)
    {
        this->addr = (long int) addr;
    }
};

struct Mem16{
    long int addr;

    Mem16(void* addr)
    {
#ifdef R64FX_DEBUG
        assert((long int)addr % 2 == 0);
#endif//R64FX_DEBUG
        this->addr = (long int) addr;
    }
};

struct Mem32{
    long int addr;

    Mem32(void* addr)
    {
#ifdef R64FX_DEBUG
        assert((long int)addr % 4 == 0);
#endif//R64FX_DEBUG
        this->addr = (long int) addr;
    }
};

struct Mem64{
    long int addr;

    Mem64(void* addr)
    {
#ifdef R64FX_DEBUG
        assert((long int)addr % 8 == 0);
#endif//R64FX_DEBUG
        this->addr = (long int) addr;
    }
};

struct Mem128{
    long int addr;

    Mem128(void* addr)
    {
#ifdef R64FX_DEBUG_JIT_STDOUT
        assert((long int)addr % 16 == 0);
#endif//R64FX_DEBUG_JIT_STDOUT
        this->addr = (long int) addr;
    }

    Mem128(float arr[4]) : Mem128((void*)arr) {}
    Mem128(int arr[4]) : Mem128((void*)arr) {}
    Mem128(unsigned int arr[4]) : Mem128((void*)arr) {}
};

struct Mem256{
    long int addr;

    Mem256(void* addr)
    {
#ifdef R64FX_DEBUG_JIT_STDOUT
        assert((long int)addr % 32 == 0);
#endif//R64FX_DEBUG_JIT_STDOUT
        this->addr = (long int) addr;
    }
};


struct Base{
    GPR64 reg;

    explicit Base(GPR64 reg) : reg(reg) {}
};


struct Disp8{
    unsigned char byte;

    explicit Disp8(unsigned char byte) : byte(byte) {}
};


const unsigned char Scale1 = b00;
const unsigned char Scale2 = b01;
const unsigned char Scale4 = b10;
const unsigned char Scale8 = b11;


class CmpCode{
    unsigned int _code;

#ifdef R64FX_DEBUG_JIT_STDOUT
    static const char* names[];
#endif//R64FX_DEBUG_JIT_STDOUT

public:
    CmpCode(unsigned int code) : _code(code) {}

    inline unsigned int code() const { return _code; }

#ifdef R64FX_DEBUG_JIT_STDOUT
    inline const char* name() const { return names[code()]; }
#endif//R64FX_DEBUG_JIT_STDOUT
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
unsigned char Shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3);


class CodeBuffer{
    unsigned char*  m_begin       = nullptr;
    unsigned char*  m_end         = nullptr;
    int             m_page_count  = 0;

public:
    CodeBuffer(int npages = 1);

    ~CodeBuffer();

    inline void rewind() 
    {
        m_end = m_begin;
    }

    /* Pointer to the beginning of the buffer. */
    inline unsigned char* codeBegin() const { return m_begin; }

    /* Pointer to the byte past the end of the written bytes. */
    inline unsigned char* codeEnd() const { return m_end; }

    inline void setEnd(void* addr) { m_end = (unsigned char*) addr; }

    inline int pageCount() const { return m_page_count; }

    inline int byteCount() const { return m_page_count * memory_page_size(); }

    inline int bytesUsed() const { return int(m_end - m_begin); }

    inline int bytesAvailable() const { return byteCount() - bytesUsed(); }

    CodeBuffer &operator<<(unsigned char byte);

    inline CodeBuffer &operator<<(Imm8 imm)
    {
        return operator<<(imm.u);
    }

    CodeBuffer &operator<<(Imm16 imm);

    CodeBuffer &operator<<(Imm32 imm);

    CodeBuffer &operator<<(Imm64 imm);
};


class Assembler{
    CodeBuffer* m_bytes = nullptr;

public:
    Assembler(CodeBuffer* bytes) : m_bytes(bytes)
    {

    }

    inline CodeBuffer* codeBuffer() const
    {
        return m_bytes;
    }

    inline unsigned char* codeBegin()
    {
        return m_bytes->codeBegin();
    }

    inline unsigned char* ip()
    {
        return m_bytes->codeEnd();
    }

    inline void setIp(unsigned char* addr)
    {
        m_bytes->setEnd(addr);
    }

    inline void rewindIp()
    {
        m_bytes->setEnd(m_bytes->codeBegin());
    }

    /* Insert one or more nop instructions. */
    inline void nop(int count = 1)
    {
        while(count--) 
        {
#ifdef R64FX_DEBUG_JIT_STDOUT
            std::cout << (void*)ip() << " nop\n";
#endif//R64FX_DEBUG_JIT_STDOUT
            *m_bytes << 0x90;
        }
    }

    inline void ret()
    {
#ifdef R64FX_DEBUG_JIT_STDOUT
        std::cout << (void*)ip() << " ret\n";
#endif//R64FX_DEBUG_JIT_STDOUT
        *m_bytes << 0xC3;
    }

    inline void rdtsc()
    {
#ifdef R64FX_DEBUG_JIT_STDOUT
        std::cout << (void*)ip() << " rdtsc\n";
#endif//R64FX_DEBUG_JIT_STDOUT
        *m_bytes << 0x0F << 0x31;
    }

    inline void rdpmc() 
    { 
#ifdef R64FX_DEBUG_JIT_STDOUT
        std::cout << (void*)ip() << " rdpmc\n";
#endif//R64FX_DEBUG_JIT_STDOUT
        *m_bytes << 0x0F << 0x33;
    }

    void mov(GPR64 reg, Imm32 imm);
    void mov(GPR64 reg, Imm64 imm);
    void mov(GPR64 dst, GPR64 src);
    void mov(GPR64 reg, Mem64 mem);
    void mov(Mem64 mem, GPR64 reg);
    void mov(GPR64 reg, Base base, Disp8 disp);
    void mov(Base base, Disp8 disp, GPR64 reg);

    void add(GPR64 reg, Imm32 imm);
    void add(GPR64 dst, GPR64 src);
    void add(GPR64 reg, Mem64 mem);
    void add(Mem64 mem, GPR64 reg);
    void add(GPR64 reg, Base base, Disp8 disp);
    void add(Base base, Disp8 disp, GPR64 reg);

    void sub(GPR64 dst, GPR64 src);
    void sub(GPR64 reg, Imm32 imm);
    void sub(GPR64 reg, Mem64 mem);
    void sub(Mem64 mem, GPR64 reg);
    void sub(GPR64 reg, Base base, Disp8 disp);
    void sub(Base base, Disp8 disp, GPR64 reg);

    void push(GPR64 reg);
    void pop(GPR64 reg);

    void cmp(GPR64 reg, Imm32 imm);
    void jmp(Mem8 mem);
    void jnz(Mem8 mem);
    void jz(Mem8 mem);
    void je(Mem8 mem);
    void jne(Mem8 mem);
    void jl(Mem8 mem);

    void movaps(Xmm dst, Xmm src);
    void movaps(Xmm reg, Mem128 mem);
    void movaps(Mem128, Xmm reg);
    void movaps(Xmm reg, Base base, Disp8 disp);
    void movaps(Base base, Disp8 disp, Xmm reg);

    void movups(Xmm dst, Xmm src);
    void movups(Xmm reg, Mem128 mem);
    void movups(Mem128, Xmm reg);
    void movups(Xmm reg, Base base, Disp8 disp);
    void movups(Base base, Disp8 disp, Xmm reg);

private:
    void sse_ps_instruction(unsigned char second_opcode, Xmm dst, Xmm src);
    void sse_ps_instruction(unsigned char second_opcode, Xmm reg, Mem128 mem);
    void sse_ps_instruction(unsigned char second_opcode, Xmm reg, Base base, Disp8 disp);

    void sse_ss_instruction(unsigned char third_opcode, Xmm dst, Xmm src);
    void sse_ss_instruction(unsigned char third_opcode, Xmm reg, Mem32 mem);
    void sse_ss_instruction(unsigned char third_opcode, Xmm reg, Base base, Disp8 disp);

public:
    void addps(Xmm dst, Xmm src);
    void addps(Xmm reg, Mem128 mem);
    void addps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void addss(Xmm dst, Xmm src);
    void addss(Xmm reg, Mem32 mem);
    void addss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void subps(Xmm dst, Xmm src);
    void subps(Xmm reg, Mem128 mem);
    void subps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void subss(Xmm dst, Xmm src);
    void subss(Xmm reg, Mem32 mem);
    void subss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void mulps(Xmm dst, Xmm src);
    void mulps(Xmm reg, Mem128 mem);
    void mulps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void mulss(Xmm dst, Xmm src);
    void mulss(Xmm reg, Mem32 mem);
    void mulss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void divps(Xmm dst, Xmm src);
    void divps(Xmm reg, Mem128 mem);
    void divps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void divss(Xmm dst, Xmm src);
    void divss(Xmm reg, Mem32 mem);
    void divss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rcpps(Xmm dst, Xmm src);
    void rcpps(Xmm reg, Mem128 mem);
    void rcpps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rcpss(Xmm dst, Xmm src);
    void rcpss(Xmm reg, Mem32 mem);
    void rcpss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void sqrtps(Xmm dst, Xmm src);
    void sqrtps(Xmm reg, Mem128 mem);
    void sqrtps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void sqrtss(Xmm dst, Xmm src);
    void sqrtss(Xmm reg, Mem32 mem);
    void sqrtss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rsqrtps(Xmm dst, Xmm src);
    void rsqrtps(Xmm reg, Mem128 mem);
    void rsqrtps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rsqrtss(Xmm dst, Xmm src);
    void rsqrtss(Xmm reg, Mem32 mem);
    void rsqrtss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void maxps(Xmm dst, Xmm src);
    void maxps(Xmm reg, Mem128 mem);
    void maxps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void maxss(Xmm dst, Xmm src);
    void maxss(Xmm reg, Mem32 mem);
    void maxss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void minps(Xmm dst, Xmm src);
    void minps(Xmm reg, Mem128 mem);
    void minps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void minss(Xmm dst, Xmm src);
    void minss(Xmm reg, Mem32 mem);
    void minss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void andps(Xmm dst, Xmm src);
    void andps(Xmm reg, Mem128 mem);
    void andps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void andnps(Xmm dst, Xmm src);
    void andnps(Xmm reg, Mem128 mem);
    void andnps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void orps(Xmm dst, Xmm src);
    void orps(Xmm reg, Mem128 mem);
    void orps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void xorps(Xmm dst, Xmm src);
    void xorps(Xmm reg, Mem128 mem);
    void xorps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void cmpps(CmpCode kind, Xmm dst, Xmm src);
    void cmpps(CmpCode kind, Xmm reg, Mem128 mem);
    void cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp = Disp8(0));

    void cmpss(CmpCode kind, Xmm dst, Xmm src);
    void cmpss(CmpCode kind, Xmm reg, Mem128 mem);
    void cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp = Disp8(0));

    inline void cmpltps(Xmm dst, Xmm src) { cmpps(LT, dst, src); }
    inline void cmpltps(Xmm reg, Mem128 mem) { cmpps(LT, reg, mem); }

    inline void cmpnltps(Xmm dst, Xmm src) { cmpps(NLT, dst, src); }
    inline void cmpnltps(Xmm reg, Mem128 mem) { cmpps(NLT, reg, mem); }

    inline void cmpeqps(Xmm dst, Xmm src) { cmpps(EQ, dst, src); }
    inline void cmpeqps(Xmm reg, Mem128 mem) { cmpps(EQ, reg, mem); }

    void movss(Mem32 mem, Xmm reg);
    void movss(Xmm reg, Mem32 mem);

    void shufps(Xmm dst, Xmm src, unsigned char imm);
    void shufps(Xmm reg, Mem128 mem, unsigned char imm);

    void pshufd(Xmm dst, Xmm src, unsigned char imm);
    void pshufd(Xmm reg, Mem128 mem, unsigned char imm);
    void pshufd(Xmm reg, Base base, Disp8 disp, unsigned char imm);

    void cvtps2dq(Xmm dst, Xmm src);
    void cvtps2dq(Xmm reg, Mem128 mem);
    void cvtps2dq(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void cvtdq2ps(Xmm dst, Xmm src);
    void cvtdq2ps(Xmm reg, Mem128 mem);
    void cvtdq2ps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void paddd(Xmm dst, Xmm src);
    void paddd(Xmm reg, Mem128 mem);
    void paddd(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void psubd(Xmm dst, Xmm src);
    void psubd(Xmm reg, Mem128 mem);
    void psubd(Xmm reg, Base base, Disp8 disp = Disp8(0));

};//Assembler

}//namespace r64fx


#endif//X86_64_JIT_ASSEMBLER_H
