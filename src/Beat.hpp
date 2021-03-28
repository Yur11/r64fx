#ifndef R64FX_BEAT_HPP
#define R64FX_BEAT_HPP

#include "Debug.hpp"

struct Beat{
    long bits = 0;

    Beat(long bits = 0) : bits(bits) {}

    Beat(long whole, long fraction) : bits(whole << 32 | fraction)
    {
        R64FX_DEBUG_ASSERT((whole & 0xFFFFFFFF) == 0);
        R64FX_DEBUG_ASSERT((fraction & 0xFFFFFFFF) == 0);
    }

    inline long whole() const { return bits >> 32; }

    inline long fraction() const { return bits & 0xFFFFFFFF; }
};

inline Beat operator+(Beat a, Beat b) { return Beat(a.bits + b.bits); }
inline Beat operator-(Beat a, Beat b) { return Beat(a.bits - b.bits); }

inline Beat operator+=(Beat a, Beat b) { return Beat(a.bits += b.bits); }
inline Beat operator-=(Beat a, Beat b) { return Beat(a.bits -= b.bits); }

inline bool operator==(Beat a, Beat b) { return a.bits == b.bits; }
inline bool operator!=(Beat a, Beat b) { return a.bits != b.bits; }
inline bool operator>=(Beat a, Beat b) { return a.bits >= b.bits; }
inline bool operator<=(Beat a, Beat b) { return a.bits <= b.bits; }

inline bool operator>(Beat a, Beat b) { return a.bits > b.bits; }
inline bool operator<(Beat a, Beat b) { return a.bits < b.bits; }

#endif//R64FX_BEAT_HPP
