#ifndef R64FX_JIT_PROCEDURES_HPP
#define R64FX_JIT_PROCEDURES_HPP

#include "jit.hpp"

#define PROC_ADDR(P) (begin() + m_offset.P)

#define PROC_TUPLE(P, A) Tuple(begin() + m_offset.P, A);

namespace r64fx{

/* Pointer + ScalarCount, captured in a single type */
template<typename T, long ScalarCount> struct VecPtr{
    long ptr = nullptr;
    VecPtr(T* ptr) : ptr(long(ptr)) {}
    constexpr static long BytesPerVector = ScalarCount * sizeof(T);
    inline auto operator+(long offset) { ptr += offset; return *this; }
    inline operator long() const { return ptr; }
};

typedef VecPtr<float, 4> VecF4;
typedef VecPtr<int,   4> VecI4;
typedef VecPtr<float, 8> VecF8;
typedef VecPtr<int,   8> VecI8;


/* Compile-time recursive tuple structure. */
template<typename... Args> class Tuple{
    /* Recursive tuple-like structure for packing data into JitProc::Sequence */
    template<typename X, typename... XS>  struct P    { X x; P<XS...> xs; P(X x, XS... xs) : x(x), xs(xs...) {} };
    template<typename X>                  struct P<X> { X x;              P(X x)           : x(x)            {} };

    P<Args...> pack;

public:
    Tuple(Args... args) : pack(args...) {}
};


/* Collection of jit procedures that can be executed in a sequence */
class JitProc : Assembler{
    struct{ long
        Exit, AddF4, MulF4, OscTaylorCosF4, OscTaylorCosF8
    ;} m_offset;

    template<typename... XS> inline auto pack(long iteration_size, long iteration_count, XS... xs)
        { return Tuple(-iteration_size*iteration_count, pack_(iteration_count, xs...)); }

    template<typename X, typename... XS> inline auto pack_(long iteration_count, X x, XS... xs)
    {
        if constexpr(sizeof...(XS) > 0)
            return Tuple(x + (iteration_count - 1) * X::BytesPerVector, pack_(iteration_count, xs...));
        else
            return Tuple(x + (iteration_count - 1) * X::BytesPerVector);
    }

    template<typename... GPRS> inline void unpack(GPRS... gprs)
        { unpack_<8>(gprs...); ADD(rbp, Imm8(sizeof...(GPRS))); }

    template<long D, typename GPR, typename... GPRS> inline void unpack_(GPR gpr, GPRS... gprs)
    {
        MOV(gpr, Base(rdi) + Index(rbp)*8 + Disp(D));
        if constexpr(sizeof...(GPRS) > 0) unpack_<D+8>(gprs...);
    }

    void genProc_AddF4();
    void genProc_MulF4();
    void genProc_OscTaylorCosF4();
    void genProc_OscTaylorCosF8();

    void loop(GPR64 counter, long increment, GPR64 jump_ptr, GPR64 jump_next);

public:
    /* A buffer that describes a sequence of procedures to be executed */
    class Sequence{
        MemoryBuffer m_buffer;

    public:
        /* Push new data into JitProc::Sequence */
        template<typename... XS> inline void push(const Tuple<XS...> &data)
            { new(m_buffer.grow(sizeof(data))) Tuple<XS...>(data); }

        /* Overloaded for convenence */
        template<typename... XS> inline auto &operator<<(const Tuple<XS...> &data)
            { push(data); return *this; }

        /* Execute JitProc::Sequence
         *
         * Make sure to use the same JitProc instance that was used to genetate this JitProc::Sequence object.
         */
        inline long exec(JitProc* jp, long nframes)
        {
            R64FX_DEBUG_ASSERT(jp); R64FX_DEBUG_ASSERT(nframes > 0);

            auto proc = (long(*)(void*, long)) ((Assembler*)jp)->begin();
            return proc(m_buffer.begin(), -nframes);
        }

        inline void clear() { m_buffer.resize(0); }
    };

    JitProc();

    inline auto procExit() { return Tuple(PROC_ADDR(Exit)); }

    inline auto procAddF4(long count, VecF4 src1, VecF4 src2, VecF4 dst)
        { return PROC_TUPLE(AddF4, pack(16, count, src1, src2, dst)); }

    inline auto procMulF4(long count, VecF4 src1, VecF4 src2, VecF4 dst)
        { return PROC_TUPLE(MulF4, pack(16, count, src1, src2, dst)); }

    inline auto procOscTaylorCosF4(long count, VecI4 clock, VecI4 clock_change, VecF4 out, VecF4 coeffs)
        { return PROC_TUPLE(OscTaylorCosF4, pack(16, count, clock, clock_change, out, coeffs)); }

    inline auto procOscTaylorCosF8(long count, VecI8 clock, VecI8 clock_change, VecF8 out, VecF8 coeffs)
        { return PROC_TUPLE(OscTaylorCosF8, pack(32, count, clock, clock_change, out, coeffs)); }
};

}//namespace r64fx

#undef PROC_ADDR
#endif//R64FX_JIT_PROCEDURES_HPP
