#ifndef R64FX_TEST_HPP
#define R64FX_TEST_HPP

#include <iostream>

#define R64FX_EXPECT_EQ(expected, got) { auto evaled = (got); if(!expect_eq(expected, evaled)) return false; }

namespace r64fx{

template<typename T1, typename T2> bool expect_eq(T1 expected, T2 got)
{
    if(expected != got)
    {
        std::cout << "Expected " << expected << ", Got " << got << "!\n";
        return false;
    }
    return true;
}

}//namespace r64fx

#endif//R64FX_TEST_HPP
