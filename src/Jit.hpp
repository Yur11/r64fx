#ifndef R64FX_JIT_X86_HPP
#define R64FX_JIT_X86_HPP

#include "Debug.hpp"
#include "MemoryUtils.hpp"
#include <sys/mman.h>

namespace r64fx{

class Reg{
    unsigned char m_bits = 0;

public:
    Reg() {}

    explicit Reg(unsigned char bits) : m_bits(bits) {}

    inline unsigned char bits() const { return m_bits; }

    inline unsigned int lowBits() const { return m_bits & 0x7; }

    inline bool prefixBit() const { return m_bits & 0x8; }

    inline constexpr static unsigned char rexwBit () { return 0; }
};

class GPR : public Reg
    { public: using Reg::Reg; };

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


#define R64FX_DEF_IMM(S, T) struct Imm##S { T val; explicit Imm##S(T val) : val(val) {} };
R64FX_DEF_IMM( 8, unsigned char)
R64FX_DEF_IMM(16, unsigned short)
R64FX_DEF_IMM(32, unsigned int)
R64FX_DEF_IMM(64, unsigned long)
#undef R64FX_DEF_IMM

auto Addr(void* addr) { return Imm64((unsigned long) addr); }

auto Shuf(unsigned char s0, unsigned char s1, unsigned char s2, unsigned char s3)
    { return Imm8((s3 << 6) + (s2 << 4) + (s1 << 2) + s0); }


#define R64FX_DEF_MEM(S, B) struct Mem##S\
    { unsigned long addr; Mem##S(void* addr = nullptr) : addr((unsigned long) addr)\
        { R64FX_DEBUG_ASSERT(this->addr % B == 0) }};
R64FX_DEF_MEM(  8,  1)
R64FX_DEF_MEM( 16,  2)
R64FX_DEF_MEM( 32,  4)
R64FX_DEF_MEM( 64,  8)
R64FX_DEF_MEM(128, 16)
#undef R64FX_DEF_MEM


struct Base{
    GPR64 reg;
    explicit Base(GPR64 reg) : reg(reg) {}
};

struct Index{
    GPR64 reg;
    explicit Index(GPR64 reg) : reg(reg) {}
};

struct Scale{ int bits; explicit Scale(int bits = 0) : bits(bits) {}};
Scale Scale1(0), Scale2(1), Scale4(2), Scale8(4);

struct IndexScale{
    Index index; Scale scale;
    IndexScale(Index index, Scale scale = Scale1) : index(index), scale(scale) {}
};

struct Disp8{
    char disp; explicit Disp8(char disp) : disp(disp) {}
};

struct Disp32{
    int disp; explicit Disp32(int disp) : disp(disp) {}
};

struct BaseIndexScale{
    Base base; IndexScale is;
    BaseIndexScale(Base base, IndexScale is) : base(base), is(is) {}
};

struct BaseIndexScaleDisp8{
    BaseIndexScale bis; Disp8 disp;
    BaseIndexScaleDisp8(BaseIndexScale bis, Disp8 disp) : bis(bis), disp(disp) {}
};

struct BaseIndexScaleDisp32{
    BaseIndexScale bis; Disp32 disp;
    BaseIndexScaleDisp32(BaseIndexScale bis, Disp32 disp) : bis(bis), disp(disp) {}
};

struct BaseDisp8{
    Base base; Disp8 disp;
    BaseDisp8(Base base, Disp8 disp) : base(base), disp(disp) {}
};

struct BaseDisp32{
    Base base; Disp32 disp;
    BaseDisp32(Base base, Disp32 disp) : base(base), disp(disp) {}
};

struct IndexScaleDisp32{
    IndexScale is; Disp32 disp;
    IndexScaleDisp32(IndexScale is, Disp32 disp) : is(is), disp(disp) {}
};

inline IndexScale            operator* (Index index, Scale scale)         { return {index, scale}; }
inline BaseIndexScale        operator+ (Base base, IndexScale is)         { return {base, is}; }
inline BaseIndexScaleDisp8   operator+ (BaseIndexScale bis, Disp8  disp)  { return {bis,  disp}; }
inline BaseIndexScaleDisp32  operator+ (BaseIndexScale bis, Disp32 disp)  { return {bis,  disp}; }
inline BaseDisp8             operator+ (Base base, Disp8  disp)           { return {base, disp}; }
inline BaseDisp32            operator+ (Base base, Disp32 disp)           { return {base, disp}; }
inline IndexScaleDisp32      operator+ (IndexScale is, Disp32 disp)       { return {is, disp}; }

struct SIBD{
    unsigned char xb, mod, sib, disp_size = 0;
    int disp = 0;

    SIBD(const Base &base)
    {
        R64FX_DEBUG_ASSERT(base.reg.lowBits() != 5);
        xbsib(base);
    }

    SIBD(const BaseDisp8 &bd8)
    {
        xbsib(bd8.base);
        modisp(bd8.disp);
    }

    SIBD(const BaseDisp32 &bd32)
    {
        xbsib(bd32.base);
        modisp(bd32.disp);
    }

    SIBD(const BaseIndexScale &bis)
    {
        R64FX_DEBUG_ASSERT(bis.base.reg.lowBits() != 5);
        R64FX_DEBUG_ASSERT(bis.is.index.reg.lowBits() != 4);
        xbsib(bis);
    }

    SIBD(const BaseIndexScaleDisp8 &bisd8)
    {
        R64FX_DEBUG_ASSERT(bisd8.bis.is.index.reg.lowBits() != 4);
        xbsib(bisd8.bis);
        modisp(bisd8.disp);
    }

    SIBD(const BaseIndexScaleDisp32 &bisd32)
    {
        R64FX_DEBUG_ASSERT(bisd32.bis.is.index.reg.lowBits() != 4);
        xbsib(bisd32.bis);
        modisp(bisd32.disp);
    }

private:
    inline void xbsib(Base base)
    {
        xb = base.reg.prefixBit() ? 1:0;
        sib = 32 | base.reg.lowBits();
    }

    inline void xbsib(BaseIndexScale bis)
    {
        xb = (bis.is.index.reg.prefixBit() ? 2:0) | (bis.base.reg.prefixBit() ? 1:0);
        sib = (bis.is.scale.bits << 6) | (bis.is.index.reg.lowBits() << 3) | bis.base.reg.lowBits();
    }

    inline void modisp(Disp8 disp8)
    {
        mod = 1;
        disp = disp8.disp;
        disp_size = 1;
    }

    inline void modisp(Disp32 disp32)
    {
        mod = 2;
        disp = disp32.disp;
        disp_size = 4;
    }

public:
    SIBD(const IndexScaleDisp32 &isd32)
    {
        R64FX_DEBUG_ASSERT(isd32.is.index.reg.lowBits() != 4);
        xb = isd32.is.index.reg.prefixBit() ? 2:0;
        sib = (isd32.is.scale.bits << 6) | (isd32.is.index.reg.lowBits() << 3) | 5;
        mod = 0;
        disp = isd32.disp.disp;
        disp_size = 4;
    }
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

#   include "JitInstructions_x86.cxx"

private:
    /* Compress VEX prefix from 3 bytes to 2, if possible */
    inline int Vex(int rxb, int map_select, int w, int vvvv, int l, int pp, bool may_pack)
    {
        m_pref = 0xC40000 | (((~rxb)&7)<<13) | (map_select<<8) | (w<<7) | (((~vvvv)&0xF)<<3) | (l<<2) | pp;
        if(may_pack && !m_use_extra_byte && (m_pref & 0xFF7F80) == 0xC46100)
            { m_pref = 0xC500 | ((m_pref & 0x8000) >> 8) | (m_pref & 0x7F); m_use_extra_byte = false; }
        return m_pref;
    }

    inline unsigned char B   (Reg r)         { m_pref = (r.prefixBit()?1:0); return m_pref; }
    inline unsigned char R   (Reg r)         { m_pref = (r.prefixBit()?4:0); return m_pref; }
    inline unsigned char RB  (Reg r, Reg b)  { m_pref = (r.prefixBit()?4:0) | (b.prefixBit()?1:0); return m_pref; }
    inline unsigned char RXB (Reg r, SIBD s) { m_pref = (r.prefixBit()?4:0) | s.xb; return m_pref; }

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
    inline void DISP(const SIBD &sibd)
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
