#ifndef R64FX_TEST_HPP
#define R64FX_TEST_HPP

#include "Debug.hpp"

#define R64FX_FILE_LINE { std::cout << __FILE__ << ":" << __LINE__ << "\n"; }

#define R64FX_EXPECT_TRUE(val)\
    { if(!(val)){ R64FX_FILE_LINE std::cout << #val" Failed!\n"; return false; } }

#define R64FX_EXPECT(val) R64FX_EXPECT_TRUE(val)

#define R64FX_EXPECT_FALSE(val)\
    { if(val) { R64FX_FILE_LINE return false; } }

#define R64FX_EXPECT_EQ(expected, got)\
    { auto evaled = (got); if(!expect_eq(expected, evaled)){ R64FX_FILE_LINE return false; } }

#define R64FX_EXPECT_VEC_EQ(v1, v2, size)\
    { if(!vec_eq(v1, v2, size)) { R64FX_FILE_LINE return false; } }

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

template<typename T1, typename T2> bool vec_eq(T1* v1, T2* v2, unsigned long size)
{
    for(unsigned long i=0; i<size; i++)
    {
        if(v1[i] != v2[i])
        {
            std::cout << "Vectors differ at " << i << " -> " << v1[i] << ", " << v2[i] << "\n";
            return false;
        }
    }
    return true;
}

}//namespace r64fx

#endif//R64FX_TEST_HPP
