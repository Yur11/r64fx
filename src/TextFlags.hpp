#ifndef R64FX_TEXT_FLAGS_HPP
#define R64FX_TEXT_FLAGS_HPP

namespace r64fx{

/* Text warp modes. */
enum class TextWrap{

    /* No text wrapping. Single line mode. */
    None,

    /* Wrap at newline. */
    Newline,

    /* Mulitiple lines. Wrap at the nearest glyph or at newline. */
    Anywhere,

    /* Mulitiple lines. Wrap at the nearest whitespace or at newline. */
    Word,

    /* Mulitiple lines. Wrap at the nearest token or at newline. */
    Token
};

}//namespace r64fx

#endif//R64FX_TEXT_FLAGS_HPP