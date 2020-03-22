#ifndef R64FX_JIT_X86_HPP
#define R64FX_JIT_X86_HPP

#include "Debug.hpp"
#include "MemoryUtils.hpp"
#include <sys/mman.h>

namespace r64fx{

/* === Registers === */
class Reg{
    unsigned char m_bits = 0;

public:
    Reg() {}

    explicit Reg(unsigned char bits) : m_bits(bits) {}

    inline unsigned char bits() const { return m_bits; }

    inline unsigned int lowBits() const { return m_bits & 0x7; }

    inline bool prefBit() const { return m_bits & 0x8; }

    static constexpr bool isGPR() { return false; }
};

class GPR : public Reg
    { public: using Reg::Reg; static constexpr bool isGPR() { return true; } };

class GPR32 : public GPR
    { public: using GPR::GPR; };

class GPR64 : public GPR
    { public: using GPR::GPR; };

class Xmm : public Reg
    { public: using Reg::Reg; };

class Ymm : public Reg
    { public: using Reg::Reg; };


inline GPR32 eax(0x0), ecx(0x1), edx (0x2), ebx (0x3), esp (0x4), ebp (0x5), esi (0x6), edi (0x7),
             r8d(0x8), r9d(0x9), r10d(0xA), e11d(0xB), r12d(0xC), r13d(0xD), r14d(0xE), r15d(0xF);

inline GPR64 rax(0x0), rcx(0x1), rdx(0x2), rbx(0x3), rsp(0x4), rbp(0x5), rsi(0x6), rdi(0x7),
             r8 (0x8), r9 (0x9), r10(0xA), r11(0xB), r12(0xC), r13(0xD), r14(0xE), r15(0xF);

inline Xmm xmm0(0x0), xmm1(0x1), xmm2 (0x2), xmm3 (0x3), xmm4 (0x4), xmm5 (0x5), xmm6 (0x6), xmm7 (0x7),
           xmm8(0x8), xmm9(0x9), xmm10(0xA), xmm11(0xB), xmm12(0xC), xmm13(0xD), xmm14(0xE), xmm15(0xF);

inline Ymm ymm0(0x0), ymm1(0x1), ymm2 (0x2), ymm3 (0x3), ymm4 (0x4), ymm5 (0x5), ymm6 (0x6), ymm7 (0x7),
           ymm8(0x8), ymm9(0x9), ymm10(0xA), ymm11(0xB), ymm12(0xC), ymm13(0xD), ymm14(0xE), ymm15(0xF);


template<typename RegT> inline bool operator==(RegT a, RegT b) { return a.bits() == b.bits(); }
template<typename RegT> inline bool operator!=(RegT a, RegT b) { return a.bits() != b.bits(); }

template<typename RegT>  inline bool has_w        (RegT) { return false; }
template<>               inline bool has_w<GPR64> (GPR64) { return true; }

template<typename IntT> inline IntT operator+(IntT opcode, Reg reg) { return opcode + reg.lowBits(); }


/* === Immediates === */
#define R64FX_DEF_IMM(S, T) struct Imm##S { T val; explicit Imm##S(T val) : val(val) {} };
R64FX_DEF_IMM( 8, unsigned char)
R64FX_DEF_IMM(16, unsigned short)
R64FX_DEF_IMM(32, unsigned int)
R64FX_DEF_IMM(64, unsigned long)
#undef R64FX_DEF_IMM

/* Address immediate */
auto Addr(void* addr) { return Imm64((unsigned long) addr); }

/* Shuffle byte immediate */
auto Shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3)
    { return Imm8((s3 << 6) + (s2 << 4) + (s1 << 2) + s0); }


/* === Memory Offset operands === */
#define R64FX_DEF_MEM(S, B) struct Mem##S\
    { unsigned long addr; Mem##S(void* addr = nullptr) : addr((unsigned long) addr)\
        { R64FX_DEBUG_ASSERT(this->addr % B == 0) }};
R64FX_DEF_MEM(  8,  1)
R64FX_DEF_MEM( 16,  2)
R64FX_DEF_MEM( 32,  4)
R64FX_DEF_MEM( 64,  8)
R64FX_DEF_MEM(128, 16)
#undef R64FX_DEF_MEM


/* === (V)SIBD === */
struct Base{
    GPR64 r; explicit Base  (GPR64 r) : r(r) {}

    inline unsigned char xb() const
        { return r.prefBit() ? 1:0; }

    inline unsigned char sib() const
        { return 32 | r.lowBits(); }
};


/* Different register types can be used as Index */
template<typename R> struct IndexTmpl{
    R r; explicit IndexTmpl (R r) : r(r) {}
};

typedef IndexTmpl<GPR64>  IndexGPR64;
typedef IndexTmpl<Xmm>    IndexXmm;
typedef IndexTmpl<Ymm>    IndexYmm;

template<typename R> inline auto Index(R r) { return IndexTmpl<R>(r); }


/* Index can be scaled */
struct Scale{ int bits; explicit Scale(int bits = 0) : bits(bits) {}}
    Scale1(0), Scale2(1), Scale4(2), Scale8(4);

template<typename I> struct IndexScale{
    I i; Scale s;
    IndexScale(I i, Scale s) : i(i), s(s) {}
    IndexScale(I i) : i(i), s(Scale1) {}
};

template<typename I> auto operator*(I i, Scale s) { return IndexScale<I>(i, s); }


/* Combine Base & Scaled Index */
template<typename I> struct BaseIndexScale{
    Base b; IndexScale<I> is;

    BaseIndexScale(Base b, IndexScale<I> is) : b(b), is(is) {}

    inline unsigned char xb() const
        { return (is.i.r.prefBit() ? 2:0) | (b.r.prefBit() ? 1:0); }

    inline unsigned char sib() const
        { return (is.s.bits << 6) | (is.i.r.lowBits() << 3) | b.r.lowBits(); }
};

template<typename I>
    inline auto operator+(Base b, IndexScale<I> is)
        { return BaseIndexScale<I>(b, is); }


/* Displacement can be added to BaseIndexScale */
template<typename D> struct DispTmpl{
    D d; explicit DispTmpl(D d) : d(d) {}
};

typedef DispTmpl<char>  Disp8;
typedef DispTmpl<int>   Disp32;

template<typename I, typename D> struct BaseIndexScaleDisp{
    BaseIndexScale<I> bis; D d;
    BaseIndexScaleDisp(BaseIndexScale<I> bis, D d) : bis(bis), d(d) {}
};

template<typename I, typename D>
    auto operator+(BaseIndexScale<I> bis, D d)
        { return BaseIndexScaleDisp<I, D>(bis, d); }

template<typename I> struct IndexScaleDisp32{
    IndexScale<I> is; Disp32 d;
    explicit IndexScaleDisp32(IndexScale<I> is, Disp32 d) : is(is), d(d) {}

    inline unsigned char xb() const
        { return is.i.r.prefBit() ? 2:0; }

    inline unsigned char sib() const
        { return (is.i.s.bits << 6) | (is.i.r.lowBits() << 3) | 5; }
};

template<typename I>
    auto operator+(IndexScale<I> is, Disp32 d)
        { return IndexScaleDisp32(is, d); }


/* Encoding bits computed from (V)SIBD */
struct SIBD_Parts{
    unsigned char xb = 0, mod = 0, sib = 0, disp_size = 0;
    int disp = 0;

protected:
    SIBD_Parts(unsigned char xb, unsigned char mod, unsigned char sib, unsigned char disp_size, int disp)
        : xb(xb), mod(mod), sib(sib), disp_size(disp_size), disp(disp) {}
};


/* VSIBD */
template<typename I> struct VSIBD : public SIBD_Parts{
protected:
    VSIBD(unsigned char xb, unsigned char mod, unsigned char sib, unsigned char disp_size, int disp)
        : SIBD_Parts(xb, mod, sib, disp_size, disp) {}

public:
    VSIBD(const BaseIndexScale<I> &bis)              : VSIBD(bis.xb(),  0, bis.sib(),  0, 0)        {}
    VSIBD(const BaseIndexScaleDisp<I, Disp8>  &bisd) : VSIBD(bisd.xb(), 1, bisd.sib(), 1, bisd.d.d) {}
    VSIBD(const BaseIndexScaleDisp<I, Disp32> &bisd) : VSIBD(bisd.xb(), 2, bisd.sib(), 4, bisd.d.d) {}
    VSIBD(const IndexScaleDisp32<I> &isd)            : VSIBD(isd.xb(),  0, isd.sib(),  4, isd.d.d)  {}
};

typedef VSIBD<IndexTmpl<Xmm>> XSIBD;
typedef VSIBD<IndexTmpl<Ymm>> YSIBD;


/* Base + Disp can be used with SIBD, but not with VSIBD */
template<typename DispT> struct BaseDispTmpl{
    Base b; DispT d;
    BaseDispTmpl(Base b, DispT d) : b(b), d(d) {}
};

template<typename DispT>
    inline auto operator+(Base b, DispT d)
        { return BaseDispTmpl(b, d); }

typedef BaseDispTmpl<Disp8>   BaseDisp8;
typedef BaseDispTmpl<Disp32>  BaseDisp32;


/* SIBD reuses parts from VSIBD and adds Base + Disp */
struct SIBD : public VSIBD<IndexTmpl<GPR64>>{
    SIBD(const Base &b)        : VSIBD(b.xb(),    0, b.sib(),    0, 0)      {}
    SIBD(const BaseDisp8 &bd)  : VSIBD(bd.b.xb(), 1, bd.b.sib(), 1, bd.d.d) {}
    SIBD(const BaseDisp32 &bd) : VSIBD(bd.b.xb(), 2, bd.b.sib(), 4, bd.d.d) {}

    using VSIBD<IndexTmpl<GPR64>>::VSIBD;
};


class Assembler;

template<typename T> class JumpLabel{
    friend class Assembler;
    unsigned char *label_addr = nullptr, *imm_addr = nullptr;

protected:
    JumpLabel(Assembler* a = nullptr)
        { if(a) operator()(a);}

public:
    /* Put label at the next instruction address */
    void operator()(Assembler* a);

    inline void clear()
        { imm_addr = label_addr = 0; }

    ~JumpLabel() { clear(); }
};

class JumpLabel8  : public JumpLabel<char> { public: using JumpLabel<char> ::JumpLabel; };
class JumpLabel32 : public JumpLabel<int>  { public: using JumpLabel<int>  ::JumpLabel; };


class Assembler : public MemoryBuffer{
    unsigned char* p = nullptr;
    bool m_use_extra_byte = false;
    int m_pref = 0;

public:
    Assembler() : MemoryBuffer(1) { permitExecution(); }

    void permitExecution()   { mprotect(begin(), nbytes(), PROT_READ | PROT_WRITE | PROT_EXEC); }
    void prohibitExecution() { mprotect(begin(), nbytes(), PROT_READ | PROT_WRITE); }

    // Force the use of REX prefix or 3-byte form of VEX prefix even if shorter encoding is possible.
    // Useful for code alignemnt.
    inline void useExtraByte(bool yes) { m_use_extra_byte = yes; }

    inline void PUSH (GPR64 r) { R(r); A(1,m_pref); O<64>(m_pref); W(0x50+r); }
    inline void POP  (GPR64 r) { R(r); A(1,m_pref); O<64>(m_pref); W(0x58+r); }

    inline void JMP  (GPR64 r) { B(r); A(2,m_pref); O<64>(m_pref); W(0xFF, ModRM(3, 4, r)); }
    inline void JMP  (Mem64 m) { A(6); W(0xFF, ModRM(0, 4, 5)); RIP(m.addr); }

    inline void JMP  (JumpLabel8  &j) { A(2); W(0xEB); LABEL(j); };
    inline void JMP  (JumpLabel32 &j) { A(5); W(0xE9); LABEL(j); };

    /* Generated by gen_jit.sh script */
#   include "JitInstructions_x86.cxx"

#   define DEF_CMPPS(C, N)\
    template<typename SrcT> inline void  CMP##C##PS (Xmm d, SrcT s)          { CMPPS  (d, s, Imm8(N)); }\
    template<typename SrcT> inline void VCMP##C##PS (Xmm d, Xmm s1, SrcT s2) { VCMPPS (d, s1, s2, Imm8(N)); }\
    template<typename SrcT> inline void VCMP##C##PS (Ymm d, Ymm s1, SrcT s2) { VCMPPS (d, s1, s2, Imm8(N)); }

    DEF_CMPPS(EQ,    0)
    DEF_CMPPS(LT,    1)
    DEF_CMPPS(LE,    2)
    DEF_CMPPS(UNORD, 3)
    DEF_CMPPS(NEQ,   4)
    DEF_CMPPS(NLT,   5)
    DEF_CMPPS(NLE,   6)
    DEF_CMPPS(ORD,   7)
#   undef DEF_CMPPS

    inline void readTicks()
    {
        RDTSC ();
        SHL   (rdx, Imm8(32));
        OR    (rax, rdx);
    }

private:
    /* Compress VEX prefix from 3 bytes to 2, if possible */
    inline int Vex(int rxb, int map_select, int w, int vvvv, int l, int pp, bool may_pack)
    {
        m_pref = 0xC40000 | (((~rxb)&7)<<13) | (map_select<<8) | (w<<7) | (((~vvvv)&0xF)<<3) | (l<<2) | pp;
        if(may_pack && !m_use_extra_byte && (m_pref & 0xFF7F80) == 0xC46100)
            { m_pref = 0xC500 | ((m_pref & 0x8000) >> 8) | (m_pref & 0x7F); m_use_extra_byte = false; }
        return m_pref;
    }

    inline unsigned char B   (Reg r)
        { m_pref = (r.prefBit()?1:0); return m_pref; }

    inline unsigned char R   (Reg r)
        { m_pref = (r.prefBit()?4:0); return m_pref; }

    inline unsigned char RB  (Reg r, Reg b)
        { m_pref = (r.prefBit()?4:0) | (b.prefBit()?1:0); return m_pref; }

    inline unsigned char RXB (Reg r, SIBD_Parts s)
        { m_pref = (r.prefBit()?4:0) | s.xb; return m_pref; }

    inline unsigned char ModRM_Bits(int bits) { return bits; }
    inline unsigned char ModRM_Bits(unsigned char bits) { return bits; }
    inline unsigned char ModRM_Bits(Reg reg) { return reg.lowBits(); }

    template<typename MOD, typename R, typename RM> inline unsigned char ModRM(MOD mod, R r, RM rm)
        { return (ModRM_Bits(mod)<<6) | (ModRM_Bits(r)<<3) | ModRM_Bits(rm); }

    /* Allocate storage for instruction */
    inline void A(int nbytes, bool extra_byte = false)
        { p = grow(nbytes + (extra_byte || m_use_extra_byte ? 1:0)); }

    /* Write bytes */
    template<typename B, typename... BS> inline void W(B b, BS... bs)
        { WW<0, B, BS...>((unsigned char)b, bs...); p += sizeof...(BS)+1; }

    template<int i, typename B, typename... BS> inline void WW(B b, BS... bs)
        { p[i] = b; if constexpr(sizeof...(BS)) WW<i+1, BS...>(bs...); }

    // Write byte if is's not zero or extra byte flag is set.
    // Add optional constant.
    template<unsigned char OptConst = 0> inline void O(unsigned char b)
        { if(b || m_use_extra_byte) W(b + OptConst); m_use_extra_byte = false; }

    /* Write immediate value */
    template<typename ImmT> inline void IMM(ImmT imm) { writeImmVal(imm.val); }

    /* Write integer value byte by byte (LE) */
    template<typename IntT> inline void writeImmVal(IntT val)
    {
        if constexpr(sizeof(val) == 1)
            W(val);
        else if constexpr(sizeof(val) == 2)
            W(val & 0xFF, val >> 8);
        else if constexpr(sizeof(val) == 4)
            W(val & 0xFF, (val >> 8) & 0xFF, (val >> 16) & 0xFF, (val >> 24) & 0xFF);
        else
            W(val & 0xFF, (val >> 8) & 0xFF, (val >> 16) & 0xFF, (val >> 24) & 0xFF,
            (val >> 32) & 0xFF, (val >> 40) & 0xFF, (val >> 48) & 0xFF, (val >> 56) & 0xFF);
    }

    /* Calculate and write RIP offset */
    inline void RIP(long addr)
        { IMM(Imm32(ripImm<int>(addr))); }

    template<typename T> T ripImm(long addr)
    {
        long offset = addr - long(p) - sizeof(T);
        R64FX_DEBUG_ASSERT(offset & ~((1L<<(sizeof(T)*8))-1) == 0);
        return T(offset);
    }

    /* Write displacement */
    inline void DISP(const SIBD_Parts &sibd)
    {
        int disp = sibd.disp;
        for(int i=0; i<sibd.disp_size; i++)
        {
            p[i] = disp & 0xFF;
            disp >>= 8;
        }
        p += sibd.disp_size;
    }

    template<typename T> inline void LABEL(JumpLabel<T> &j)
    {
        R64FX_DEBUG_ASSERT(!j.imm_addr); // One label per jump!
        j.imm_addr = p;
        if(j.label_addr)
        {
            writeImmVal(ripImm<T>((long)j.label_addr));
        }
    }

public:
    template<typename T> inline void putLabel(JumpLabel<T> &j)
    {
        j.label_addr = p;
        if(j.imm_addr)
        {
            p = j.imm_addr;
            writeImmVal(ripImm<T>((long)j.label_addr - sizeof(T)));
        }
    }
};

template<typename T> void JumpLabel<T>::operator()(Assembler *a)
    { a->putLabel<T>(*this); }

}//namespace r64fx

#endif//R64FX_JIT_X86_HPP
