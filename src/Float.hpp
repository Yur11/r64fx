#ifndef R64FX_FLOAT_HPP
#define R64FX_FLOAT_HPP

namespace r64fx{

union Float32{
    float f = 0.0f;
    unsigned int n;

    Float32(float f) : f(f) {}

    Float32() {}

    inline unsigned int mantissaBits() const
    {
        return n & 0x007FFFFF;
    }

    inline unsigned int exponentBits() const
    {
        return n & 0x7F800000;
    }

    inline unsigned int signBit() const
    {
        return n & 0x80000000;
    }

    inline int exponent() const
    {
        return int(exponentBits() >> 23) - 127;
    }

    inline unsigned int mantissa() const
    {
        return mantissaBits() | 0x00800000;
    }

    inline unsigned int wholePart() const
    {
        return ((exponent() < 0) ? 0 : mantissa() >> (23 - exponent()));
    }

    inline unsigned int fractionalBits() const
    {
        return (mantissa() & ((0x00800000 >> exponent()) - 1)) << (exponent() + 1);
    }

    inline static float Nan()
    {
        Float32 u;
        u.n = 0x7FFFFFFF;
        return u.f;
    }

    inline static bool IsNan(float f)
    {
        Float32 u(f);
        return u.exponentBits() == 0x7F800000 && u.mantissaBits() != 0;
    }

    static void frac2str(unsigned int bits, char* buff, int buff_size);
};

}//namespace r64fx

#endif//R64FX_FLOAT_HPP
