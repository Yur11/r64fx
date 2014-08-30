#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#ifdef DEBUG
#include <assert.h>
#include <sstream>
#endif//DEBUG

#include "shared_sources/binary_constants.hpp"

namespace r64fx{

    
void* alloc_pages_raw(int npages);

void* alloc_alligned_raw(int alignment, int nbytes);
    
template<typename T> T alloc_pages(int npages) { return (T) alloc_pages_raw(npages); }

template<typename T> T alloc_alligned(int alignment, int nbytes) { return (T) alloc_alligned_raw(alignment, nbytes); }


/* Immediate operands. */
class Imm8{
    friend class CodeBuffer;
    unsigned char byte;
    
public:
    Imm8(unsigned char byte)
    {
        this->byte = byte;
    }
    
    inline operator unsigned char() const { return byte; }
    inline operator   signed char() const { return byte; }
};

class Imm16{
    friend class CodeBuffer;
    union{
        unsigned short word;
        unsigned char byte[2];
    }bytes;
    
public:
    explicit Imm16(unsigned short word)
    {
        bytes.word = word;
    }
    
    inline operator unsigned short() const { return bytes.word; }
    inline operator   signed short() const { return bytes.word; }
};


class Imm32{
    friend class CodeBuffer;
    union{
        unsigned int dword;
        unsigned char byte[4];
    }bytes;
    
public:
    explicit Imm32(unsigned int dword)
    {
        bytes.dword = dword;
    }
    
    inline operator unsigned int() const { return bytes.dword; }
    inline operator   signed int() const { return bytes.dword; }
};


class Imm64{
    friend class CodeBuffer;
    union{
        unsigned long qword;
        unsigned char byte[8];
    }bytes;
    
public:
    explicit Imm64(void* ptr) : Imm64((unsigned long)ptr) {}
    
    explicit Imm64(unsigned long qword)
    {
        bytes.qword = qword;
    }
    
    inline operator unsigned long int() const { return bytes.qword; }
    inline operator   signed long int() const { return bytes.qword; }
}; 


class ImmPtr{
    Imm64 imm;

public:
    explicit ImmPtr(void* ptr)
    :imm((long int)ptr)
    {
    }

    inline operator Imm64() const { return imm; }
};


class Register{
    const unsigned char _bits;

public:
    Register(const unsigned char bits) : _bits(bits) {}
    
    inline unsigned char bits() const { return _bits; }
    
    inline unsigned char code() const { return _bits; }
    
    /** @brief R or B bit of the REX prefix.*/
    inline bool prefix_bit() const { return _bits & b1000; }    
};

class GPR64 : public Register{
#ifdef DEBUG
    static const char* names[];
#endif//DEBUG
    
public:
    explicit GPR64(const unsigned char bits) : Register(bits) {}
    
#ifdef DEBUG
    inline const char* name() const { return names[Register::code()]; }
#endif//DEBUG
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


class GPR32 : public Register{
#ifdef DEBUG
    static const char* names[];
#endif//DEBUG
    
public:
    explicit GPR32(const unsigned char bits) : Register(bits) {}
    
#ifdef DEBUG
    inline const char* name() const { return names[Register::code()]; }
#endif//DEBUG
};

const GPR32
    eax(b000),
    ecx(b001),
    edx(b010),
    ebx(b011),
    esp(b100),
    ebp(b101),
    esi(b110),
    edi(b111),
    r8d (b1000),
    r9d (b1001),
    r10d(b1010),
    r11d(b1011),
    r12d(b1100),
    r13d(b1101),
    r14d(b1110),
    r15d(b1111)
;


class GPR16 : public Register{
#ifdef DEBUG
    static const char* names[];
#endif//DEBUG
    
public:
    explicit GPR16(const unsigned char bits) : Register(bits) {}
    
#ifdef DEBUG
    inline const char* name() const { return names[Register::code()]; }
#endif//DEBUG
};

const GPR16
    ax(b000),
    cx(b001),
    dx(b010),
    bx(b011),
    sp(b100),
    bp(b101),
    si(b110),
    di(b111)
;


class GPR8 : public Register{
#ifdef DEBUG
    static const char* names[];
#endif//DEBUG
    
public:
    explicit GPR8(const unsigned char bits) : Register(bits) {}
    
#ifdef DEBUG
    inline const char* name() const { return names[Register::code()]; }
#endif//DEBUG
};

const GPR8
    al(b000),
    cl(b001),
    dl(b010),
    bl(b011),
    ah(b100),
    ch(b101),
    dh(b110),
    bh(b111)
;


class Xmm : public Register{
#ifdef DEBUG
    static const char* names[];
#endif//DEBUG
    
public:
    explicit Xmm(const unsigned char bits) : Register(bits) {}
    
#ifdef DEBUG
    inline const char* name() const { return names[Register::code()]; }
#endif//DEBUG
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


class Ymm : public Register{

public:
    explicit Ymm(const unsigned char bits) : Register(bits) {}
};

const Ymm
    ymm0(b0000),
    ymm1(b0001),
    ymm2(b0010),
    ymm3(b0011),
    ymm4(b0100),
    ymm5(b0101),
    ymm6(b0110),
    ymm7(b0111),
    ymm8(b1000),
    ymm9(b1001),
    ymm10(b1010),
    ymm11(b1011),
    ymm12(b1100),
    ymm13(b1101),
    ymm14(b1110),
    ymm15(b1111)
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
#ifdef DEBUG
        assert((long int)addr % 2 == 0);
#endif//DEBUG
        this->addr = (long int) addr;
    }
};

struct Mem32{
    long int addr;
    
    Mem32(void* addr)
    {
#ifdef DEBUG
        assert((long int)addr % 4 == 0);
#endif//DEBUG
        this->addr = (long int) addr;
    }
};

struct Mem64{
    long int addr;
    
    Mem64(void* addr)
    {
#ifdef DEBUG
        assert((long int)addr % 8 == 0);
#endif//DEBUG
        this->addr = (long int) addr;
    }
};

struct Mem128{
    long int addr;
    
    Mem128(void* addr)
    {
#ifdef DEBUG
        assert((long int)addr % 16 == 0);
#endif//DEBUG
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
#ifdef DEBUG
        assert((long int)addr % 32 == 0);
#endif//DEBUG
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
    
#ifdef DEBUG
    static const char* names[];
#endif//DEBUG

public:
    CmpCode(unsigned int code) : _code(code) {}

    inline unsigned int code() const { return _code; }
    
#ifdef DEBUG
    inline const char* name() const { return names[code()]; }
#endif//DEBUG
};

/** @brief Codes used with cmpps. */
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


/** @brief Pack four 0..3 values into a sigle byte. To be used with shuffle instructions. 
 
    Each parameter defines the source scalar from which to read the data into the current position.
    For example 
       Setting s3 to 0, means moving the first scalar form the source vector into the last scalar of the destination vector.
       - shuf(0, 1, 2, 3) - no shuffle.
       - shuf(3, 2, 1, 0) - reverse order.
       - etc...
 */
unsigned char shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3);


class CodeBuffer{
    unsigned char* begin;
    unsigned char* end;
    int _npages;
    
public:
    CodeBuffer(int npages = 1);
    
    ~CodeBuffer()
    {
    }
    
    void cleanup();
    
    inline void rewind() 
    {
        end = begin;
    }
    
    /** @brief Pointer to the beginning of the buffer. */
    inline unsigned char* codeBegin() const { return begin; }
    
    /** @brief Pointer to the byte past the end of the written bytes. 
     
        This can be used to obtain memory locations for doing branching to lower addresses.
     */
    inline unsigned char* codeEnd() const { return end; }
    
    inline void setEnd(void* addr) { end = (unsigned char*) addr; }
    
    inline int npages() const { return _npages; }
        
    CodeBuffer &operator<<(unsigned char byte);

    inline CodeBuffer &operator<<(Imm8 imm)
    {
        return operator<<(imm.byte);
    }

    CodeBuffer &operator<<(Imm16 imm);
    
    CodeBuffer &operator<<(Imm32 imm);
    
    CodeBuffer &operator<<(Imm64 imm);
};


class Assembler{
public:
    CodeBuffer &bytes;
    
#ifdef DEBUG
    std::ostringstream dump;
#endif//DEBUG
    
    /** Use an external CodeBuffer. Do not free it! */
    Assembler(CodeBuffer &cb) : bytes(cb)
    {

    }

    inline void* getFun()
    {
        return bytes.codeBegin();
    }

    inline unsigned char* ip()
    {
        return bytes.codeEnd();
    }

    /** @brief Insert one or more nop instructions. */
    inline void nop(int count = 1)
    {
        while(count--) 
        {
#ifdef DEBUG
            dump << (void*)ip() << "    nop\n";
#endif//DEBUG
            bytes << 0x90;
        }
    }

    inline void ret()
    {
#ifdef DEBUG
        dump << (void*)ip() << "    ret\n";
#endif//DEBUG
        bytes << 0xC3;
    }

    inline void rdtsc() 
    {
#ifdef DEBUG
        dump << (void*)ip() << "    rdtsc\n";
#endif//DEBUG
        bytes << 0x0F << 0x31; 
    }
    
    inline void rdpmc() 
    { 
#ifdef DEBUG
        dump << (void*)ip() << "    rdpmc\n";
#endif//DEBUG
        bytes << 0x0F << 0x33; 
    }

    void add(GPR32 reg, Mem32 mem);
    void add(Mem32 mem, GPR32 reg);
    void add(GPR32 dst, GPR32 src);

    void add(GPR64 reg, Mem64 mem);
    void add(Mem64 mem, GPR64 reg);
    void add(GPR64 dst, GPR64 src);

    void add(GPR64 reg, Base base, Disp8 disp = Disp8(0));
    void add(Base base, GPR64 reg);

    void add(GPR64 reg, Imm32 imm);
    inline
    void add(GPR64 reg, unsigned int imm) { add(reg, Imm32(imm)); }

    void sub(GPR32 reg, Mem32 mem);
    void sub(Mem32 mem, GPR32 reg);
    void sub(GPR32 dst, GPR32 src);

    void sub(GPR64 reg, Mem64 mem);
    void sub(Mem64 mem, GPR64 reg);
    void sub(GPR64 dst, GPR64 src);

    void sub(GPR64 reg, Imm32 imm);
    inline
    void sub(GPR64 reg, unsigned int imm) { sub(reg, Imm32(imm)); }

    void sub(GPR64 reg, Base base);
    void sub(Base base, GPR64 reg);

    void mov(GPR32 reg, Mem32 mem);
    void mov(Mem32 mem, GPR32 reg);
    void mov(GPR32 dst, GPR32 src);
    
    void mov(GPR64 reg, Mem64 mem);
    void mov(Mem64 mem, GPR64 reg);
    void mov(GPR64 dst, GPR64 src);
    void mov(GPR64 reg, Imm32 imm);
    void mov(GPR64 reg, Imm64 imm);
    void mov(GPR64 reg, unsigned long int imm);

    void mov(GPR64 reg, Base base, Disp8 disp = Disp8(0));
    void mov(Base base, Disp8 disp, GPR64 reg);
    inline 
    void mov(Base base, GPR64 reg) { mov(base, Disp8(0), reg); }
    void mov(Base base, GPR32 reg);


    void push(GPR64 reg);

    void pop(GPR64 reg);


    void cmp(GPR64 reg, Imm32 imm);
    inline void cmp(GPR64 reg, unsigned int imm) { cmp(reg, Imm32(imm)); }

    void jmp(Mem8 mem);
    void jnz(Mem8 mem);
    void jz(Mem8 mem);
    void je(Mem8 mem);
    void jne(Mem8 mem);
    void jl(Mem8 mem);


    /* SSE */
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
    
    void movups(Xmm dst, Xmm src);
    void movups(Xmm reg, Mem128 mem);
    void movups(Xmm reg, Base base, Disp8 disp = Disp8(0));
    void movups(Mem128, Xmm reg);
    void movups(Base base, Disp8 disp, Xmm reg);
    inline void movups(Base base, Xmm reg) { movups(base, Disp8(0), reg); }

    void movaps(Xmm dst, Xmm src);
    void movaps(Xmm reg, Mem128 mem);
    void movaps(Xmm reg, Base base, Disp8 disp = Disp8(0));
    void movaps(Mem128, Xmm reg);
    void movaps(Base base, Disp8 disp, Xmm reg);
    inline void movaps(Base base, Xmm reg) { movaps(base, Disp8(0), reg); }
    
    void movss(Mem32 mem, Xmm reg);
    void movss(Xmm reg, Mem32 mem);
    
    /* ??? */
    void shufps(Xmm dst, Xmm src, unsigned char imm);
    void shufps(Xmm reg, Mem128 mem, unsigned char imm);
    /* ??? */
    
    void pshufd(Xmm dst, Xmm src, unsigned char imm);
    void pshufd(Xmm reg, Mem128 mem, unsigned char imm);
    void pshufd(Xmm reg, Base base, Disp8 disp, unsigned char imm);
    inline void pshufd(Xmm reg, Base base, unsigned char imm)
    {
        pshufd(reg, base, Disp8(0), imm);
    }
    
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