#ifndef R64FX_TEXT_PAINTER_HPP
#define R64FX_TEXT_PAINTER_HPP

#include "Rect.hpp"
#include <string>

namespace r64fx{

class Image;
class Font;


enum class TextWrap{

    /* No text wrapping. Single line mode. */
    None,

    /* Mulitiple lines. Wrap at the nearest glyph. */
    Anywhere,

    /* Mulitiple lines. Wrap at the nearest whitespace. */
    Word,

    /* Mulitiple lines. Wrap at the nearest token. */
    Token
};


class TextPainter{
    void*        m        = nullptr;

public:
    Image*        image  = nullptr;
    std::string*  text   = nullptr;
    Font*         font   = nullptr;

    TextPainter();

   ~TextPainter();

    bool isGood() const;

    /* Recalculate text flow with the given wrap_mode..
     * The width parameter is used with multi-line modes
     * to determine the wrap point. */
    void reflow(TextWrap wrap_mode, int width);

    /* As calculated by the reflow method. */
    int lineCount() const;

    /* Size of the image that can fit the text.
     * As calculated be the reflow method. */
    Size<int> textSize() const;

    /* Paint text onto image.
     * Use offset from that top-left corner. */
    void paint(Image* image, Point<int> offset = {0, 0});

    void inputUtf8(const std::string &text);
};

}//namespace

#endif//R64FX_TEXT_PAINTER_HPP