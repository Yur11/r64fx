#include "test.hpp"
#include "jit_algorithms.hpp"

using namespace std;
using namespace r64fx;


template<typename T> inline void dump(T vec, int size)
{
    int i=0;
    while(i<size)
    {
        std::cout << std::hex << std::setw(3);
        std::cout << vec[i]; i++;
        std::cout << (i == size ? "\n" : ", ");
        std::cout << std::dec;
    }
}


template<typename JitAlgorithmT> class JitAlgorithmTest : public JitAlgorithmT{
    void stub() {}

    void beginFun()
    {
        PUSH(rbp);
        PUSH(rbx);
    }

    void endFun()
    {
        POP(rbx);
        POP(rbp);
        RET();
    }

protected:
    long measureTimeOf(
        void(JitAlgorithmTest::*algorithm) (),
        void* rsi_arg = nullptr,
        void* rdi_arg = nullptr,
        void(JitAlgorithmTest::*prologue)  () = &JitAlgorithmTest::stub,
        void(JitAlgorithmTest::*epilogue)  () = &JitAlgorithmTest::stub
    )
    {
        rewindCode();
        auto fun = (long (*)(void*,void*)) codeBegin();

        beginFun();
        (this->*prologue)();
        getBeginTime();

        MOV(rcx, Imm32(1000));
        JumpLabel loop;
        mark(loop);
        (this->*algorithm)();
        SUB(rcx, Imm8(1));
        JNZ(loop);

        getEndTime();
        (this->*epilogue)();
        endFun();

        fun(rsi_arg, rdi_arg);
        return time();
    }

    R64FX_USING_JIT_METHODS(JitAlgorithmT)
};


class TestPlayback : public JitAlgorithmTest<JitAlgorithm_Playback<Assembler>>{
    Constants*  m_constants  = nullptr;
    Playhead*   m_playhead   = nullptr;
    float*      m_data       = {};

public:
    TestPlayback()
    {
        auto buff = dataEnd() - memory_page_size();

        m_constants = (Constants*)buff;
        buff += sizeof(Constants);

        m_playhead = (Playhead*)buff;
        buff += sizeof(Playhead);

        m_data = (float*)buff;

        constexpr int BuffSize = 8;

        resize(1, 1);
//         for(int i=0; i<4; i++)
//         {
//             auto data = m_data + (i*BuffSize);
//             for(int j=0; j<BuffSize; j++)
//             {
//                 buff[i][j] = float(i) + float(j) / float(BuffSize);
//                 cout << buff[i][j] << (j == BuffSize-1 ? "\n" : ", ");
//             }
//         }
    }

    bool test()
    {
        rewindCode();
        auto fun = (void (*)()) codeBegin();

//         build(m_constants, );

        fun();
        return true;
    }

    int runTests()
    {
        bool ok = test();
        return ok;
    }

    int measureTimes()
    {
        return 0;
    }
};


void show_usage(char* name)
{
    cout << "Usage: " << name << " (test|time)\n";
}


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        show_usage(argv[0]);
    }
    else
    {
        if(string(argv[1]) == "test")
        {
            return TestPlayback().runTests();
        }
        else if(string(argv[1]) == "time")
        {
            return TestPlayback().measureTimes();
        }
        else
        {
            show_usage(argv[0]);
            return 0;
        }
    }
    return 0;
}
