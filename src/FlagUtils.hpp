#ifndef R64FX_FLAG_UTILS_HPP
#define R64FX_FLAG_UTILS_HPP

/* Type-Safe flags. */

#include "Debug.hpp"

#define R64FX_FLAG_TYPE(Mask) public FlagType<Mask>\
    { public: using FlagType::FlagType; constexpr static unsigned long mask() { return Mask; } }

#define R64FX_FLAG_OR_OPERATORS(Compound, Constituent1, Constituent2)\
    static_assert((Constituent1::mask() & Constituent2::mask()) == 0, #Constituent1 " & " #Constituent2 " overlap!");\
    inline Compound operator|(Constituent1 c1, Constituent2 c2) { return Compound( c1.bits() | c2.bits() ); }\
    inline Compound operator|(Constituent2 c2, Constituent1 c1) { return Compound( c1.bits() | c2.bits() ); }

#define R64FX_FLAG_AND_OPERATORS(Compound, Constituent)\
    inline Constituent operator&(Compound c1, Constituent c2) { return Constituent( c1.bits() & c2.bits() ); }\
    inline Constituent operator&(Constituent c1, Compound c2) { return Constituent( c1.bits() & c2.bits() ); }\

#define R64FX_FLAG_OPERATORS(Compound, Constituent1, Constituent2)\
    R64FX_FLAG_OR_OPERATORS(Compound, Constituent1, Constituent2)\
    R64FX_FLAG_AND_OPERATORS(Compound, Constituent1)\
    R64FX_FLAG_AND_OPERATORS(Compound, Constituent2)

#define R64FX_COMPOUND_FLAG_TYPE(Compound, Constituent1, Constituent2)\
    class Compound : R64FX_FLAG_TYPE(Constituent1::mask() | Constituent2::mask());\
    R64FX_FLAG_OPERATORS(Compound, Constituent1, Constituent2);

#define R64FX_COMBINE_3_FLAG_TYPES(Compound, Constituent1, Constituent2, Constituent3)\
    R64FX_COMPOUND_FLAG_TYPE(Constituent1##Constituent2, Constituent1, Constituent2);\
    R64FX_COMPOUND_FLAG_TYPE(Constituent1##Constituent3, Constituent1, Constituent3);\
    R64FX_COMPOUND_FLAG_TYPE(Constituent2##Constituent3, Constituent2, Constituent3);\
    R64FX_FLAG_OPERATORS(Compound, Constituent1##Constituent2, Constituent3);\
    R64FX_FLAG_OPERATORS(Compound, Constituent1##Constituent3, Constituent2);\
    R64FX_FLAG_OPERATORS(Compound, Constituent2##Constituent3, Constituent1);


namespace r64fx{

template<unsigned long Mask> class FlagType{
    unsigned long m_bits = Mask; //We can use default instance for masking.

public:
    FlagType() {}

    explicit FlagType(unsigned long bits) : m_bits(bits) { R64FX_DEBUG_ASSERT((bits & ~Mask) == 0); };

    inline unsigned long bits() const { return m_bits; }

    inline operator bool() const { return m_bits; }

    inline bool operator==(FlagType<Mask> other) { return m_bits == other.bits(); }

    inline bool operator!=(FlagType<Mask> other) { return m_bits != other.bits(); }
};

}//namespace

#endif//R64FX_FLAG_UTILS_HPP
