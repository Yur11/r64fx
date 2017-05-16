#ifndef R64FX_FLAG_BITS_HPP
#define R64FX_FLAG_BITS_HPP

namespace r64fx{

/* Base class for type-safe flags. */
template<class FlagsT> class FlagBits{
    unsigned long m_bits = 0;

public:
    FlagBits() {}

    explicit FlagBits(unsigned long bits) : m_bits(bits) {}

    inline unsigned long bits() const { return m_bits; }

    inline FlagsT operator|(const FlagsT &other) const { return FlagsT(bits() | other.bits()); }
    inline FlagsT operator&(const FlagsT &other) const { return FlagsT(bits() & other.bits()); }

    inline operator bool() const { return m_bits != 0; }

protected:
    inline void setBits(unsigned long bits) { m_bits = bits; }
};

}//namespace r64fx

#endif//R64FX_FLAG_BITS_HPP
