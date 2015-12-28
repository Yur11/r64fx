#ifndef R64FX_TEXT_FLAGS_HPP
#define R64FX_TEXT_FLAGS_HPP

namespace r64fx{

/* Text warp modes. */
namespace TextWrap{
    class Mode{
        unsigned int m_bits = 0;

    public:
        explicit Mode(unsigned int bits) : m_bits(bits) {}

        inline unsigned int bits() const { return m_bits & 7; }
    };

    inline bool operator==(const Mode &a, const Mode &b) { return a.bits() == b.bits(); }

    inline bool operator!=(const Mode &a, const Mode &b) { return !operator==(a, b); }

    /* No text wrapping. Single line mode. */
    const Mode None(0);

    /* Wrap at newline. */
    const Mode Newline(1);

    /* Mulitiple lines. Wrap at the nearest glyph or at newline. */
    const Mode Anywhere(3);

    /* Mulitiple lines. Wrap at the nearest whitespace or at newline. */
    const Mode Word (4);

    /* Mulitiple lines. Wrap at the nearest token or at newline. */
    const Mode Token(5);
}//namespace TextWrap;

}//namespace r64fx

#endif//R64FX_TEXT_FLAGS_HPP