#ifndef R64FX_JIT_PROCEDURES_HPP
#define R64FX_JIT_PROCEDURES_HPP

#include "jit.hpp"

namespace r64fx{

/* Pointer + ScalarCount, captured in a single type */
template<typename T, long ScalarCount> struct VecPtr{
    long ptr = nullptr;
    VecPtr(T* ptr) : ptr(long(ptr)) {}
    constexpr static long BytesPerVector = ScalarCount * sizeof(T);
    inline auto operator+(long offset) { ptr += offset; return *this; }
};

/* A collection of VecPtr instances + iteration_count
 *
 * Adjusts pointers and counter_value to work with loop() optimization.
 * See: JitProc::loop() method.
 */
template<long BytesPerVector, typename... Ptrs> class VecPtrPack{
    /* Recursive tuple-like structure for packing data into JitProc::Sequence */
    template<typename X, typename... XS> struct Pack{
        X x; Pack<XS...> xs;

        Pack(long offset, X x, XS... xs) : x(x + offset), xs(offset, xs...)
        {
            static_assert(alignof(X) == 8);
            static_assert(X::BytesPerVector >= BytesPerVector);
        }
    };

    template<typename X> struct Pack<X>{ X x; Pack(long offset, X x) : x(x + offset) {} };

    long counter_value = 0; Pack<Ptrs...> pack;

public:
    VecPtrPack(long iteration_count, Ptrs... ptrs)
        : counter_value(-(iteration_count * BytesPerVector))
        , pack((iteration_count - 1) * BytesPerVector, ptrs...)
        { R64FX_DEBUG_ASSERT(iteration_count > 0); }
};

typedef VecPtr<float, 4> VecF4;

typedef VecPtrPack<16, VecF4, VecF4, VecF4> VecPack3F4;


/* Collection of jit procedures that can be executed in a sequence */
class JitProc : Assembler{
public:
    /* A buffer that describes a sequence of procedures to be executed */
    class Sequence{
        MemoryBuffer m_buffer;

    public:
        /* Helper class for packing data into JitProc::Sequence and unpacking that data into registers
         *
         * Used to enforce type safety and consistency.
         */
        template<typename... Args> class Data{
            friend class JitProc;

            /* Recursive tuple-like structure for packing data into JitProc::Sequence */
            template<typename X, typename... XS> struct Pack{
                X x; Pack<XS...> xs;

                Pack(X x, XS... xs) : x(x), xs(xs...)
                {
                    static_assert(alignof(X) == 8);
                }
            };

            template<typename X> struct Pack<X>{ X x; Pack(X x) : x(x) {} };

            Pack<void*, Args...> pack; Data(void* proc, Args... args) : pack(proc, args...) {}

            /* Functor for unpacking JitProc::Sequence::Data into registers */
            class Unpack{
                Assembler* as = nullptr;

            public:
                Unpack(Assembler* as) : as(as) {}

                template<typename... Regs> inline void operator()(Regs... regs)
                {
                    static_assert(sizeof...(Regs) * 8 == sizeof(Pack<void*, Args...>)); // Args + next proc addr
                    unpack<8, Regs...>(regs...);
                    as->ADD(rbp, Imm8(sizeof...(Regs)));
                }

            private:
                template<long D, typename Reg, typename... Regs>inline void unpack(Reg reg, Regs... regs)
                {
                    R64FX_DEBUG_ASSERT(reg != rdi);
                    R64FX_DEBUG_ASSERT(reg != rsi);
                    as->MOV(reg, Base(rdi) + Index(rbp)*8 + Disp(D));
                    if constexpr(sizeof...(Regs) > 0) unpack<D+8, Regs...>(regs...);
                }
            };
        };

        /* Push new data into JitProc::Sequence */
        template<typename... XS> inline void push(const Data<XS...> &data)
            { new(m_buffer.grow(sizeof(data))) Data<XS...>(data); }

        /* Overloaded for convenence */
        template<typename... XS> inline auto &operator<<(const Data<XS...> &data)
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

    JitProc()
    {
        PUSH (rbx);
        PUSH (rbp);
        PUSH (rdi);
        PUSH (rsi);
        PUSH (r12);

        XOR  (rax, rax);

        R64FX_JIT_LABEL(main_loop); //Iterate over nframes passed via rsi
        MOV  (rdx, Base(rdi));      //Load first proc addr
        XOR  (rbp, rbp);            //Reset JitProcSequence index
        JMP  (rdx);                 //Jump to first proc

        /* ------------------------------------------- */

        genProc_Exit(); // Save Proc Exit address
        ADD  (rsi, Imm8(1));
        JNZ  (main_loop);

        POP  (r12);
        POP  (rsi);
        POP  (rdi);
        POP  (rbp);
        POP  (rbx);
        RET  ();

        genProc_Add();
        genProc_Mul();
        permitExecution();
    }

private:
    /* Generate procedure loop entry point.
     *
     * counter    - must contain initial negative value, use as an index register
     * increment  - added to counter with each iteration
     * jump_ptr   - stores the address of the loop entry point and
     *              is assigned the value of jump_next upon counter reaching zero
     *
     * Close with JMP(jump_ptr)
     *
     * NOTE: Pointers passed as JitProc arguments must be adjusted to make this loop work.
     *       Base pointer of each buffer must point to the address of the last vector,
     *       because at the last iteration the counter index register becomes zero.
     */
    inline void loop(GPR64 counter, long increment, GPR64 jump_ptr, GPR64 jump_next)
    {
        leaNextInstruction(jump_ptr);
        ADD(counter, Imm8(increment));
        CMOVZ(jump_ptr, jump_next);
    }

    /* Procedure boilerplate code */
#define R64FX_JIT_PROC(P, ...)                                                      \
    typedef Sequence::Data<__VA_ARGS__> SeqData##P;                                 \
                                                                                    \
    public:                                                                         \
        /* Generate JitProc::Sequence::Data with the address of this proc,          \
         * followed by optional arguments,                                          \
         * to be passed into JitProc::Sequence::pack() method.                      \
         */                                                                         \
        template<typename... XS> inline auto proc##P(XS... xs)                      \
            { return SeqData##P(begin() + m_offset_##P, xs...); }                   \
                                                                                    \
    private:                                                                        \
        long m_offset_##P = 0; /* Offset of this proc in the code buffer */         \
                                                                                    \
        /* Save proc m_proc_offset##P, generate proc code. */                       \
        inline void genProc_##P()                                                   \
        {                                                                           \
            m_offset_##P = bytesUsed();                                             \
            SeqData##P::Unpack unpack(this);                                        \
            _genProc##P(unpack);                                                    \
        }                                                                           \
                                                                                    \
        /* Proc generator function, passed Unpack functor                           \
         * to safely get data from JitProc::Sequence                                \
         */                                                                         \
        inline void _genProc##P(SeqData##P::Unpack unpack)                          \
            /* { Follow with function body! } */

    R64FX_JIT_PROC(Exit) {}

    R64FX_JIT_PROC(Add, VecPack3F4)
    {
        unpack(rax, rcx, rdx, rbx, r9);
        loop(rax, 16, r8, r9);
            MOVAPS (xmm0, Base(rcx) + Index(rax));
            ADDPS  (xmm0, Base(rdx) + Index(rax));
            MOVAPS (Base(rbx) + Index(rax), xmm0);
        JMP(r8);
    }

    R64FX_JIT_PROC(Mul, VecPack3F4)
    {
        unpack(rax, rcx, rdx, rbx, r9);
        loop(rax, 16, r8, r9);
            MOVAPS (xmm0, Base(rcx) + Index(rax));
            MULPS  (xmm0, Base(rdx) + Index(rax));
            MOVAPS (Base(rbx) + Index(rax), xmm0);
        JMP(r8);
    }

#undef R64FX_JIT_PROC
};

}//namespace r64fx

#endif//R64FX_JIT_PROCEDURES_HPP
