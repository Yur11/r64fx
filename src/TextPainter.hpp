#ifndef R64FX_TEXT_PAINTER_HPP
#define R64FX_TEXT_PAINTER_HPP

#include "Rect.hpp"
#include "Font.hpp"
#include "TextFlags.hpp"
#include <string>
#include <vector>

namespace r64fx{

class GlyphEntry{
    Font::Glyph* m_glyph;
    int          m_index; //Index in text.

public:
    GlyphEntry(Font::Glyph* glyph, int index)
    : m_glyph(glyph)
    , m_index(index)
    {}

    inline Font::Glyph* glyph() const { return m_glyph; }

    inline int index() const { return m_index; }

    inline std::string text() const { return m_glyph->text(); }
};


struct GlyphLine{
    int m_y;
    int m_begin;   //Index of the first glyph in line.
    int m_end;     //Index past the last glyph in line,

    GlyphLine(int y, int begin, int end)
    : m_y(y)
    , m_begin(begin)
    , m_end(end)
    {}

    inline int y() const { return m_y; }

    inline void setBegin(int begin) { m_begin = begin; }

    inline int begin() const { return m_begin; }

    inline void setEnd(int end) { m_end = end; }

    inline int end() const { return m_end; }
};


class TextPainter{
    std::vector<GlyphEntry> m_glyphs;
    std::vector<GlyphLine>  m_lines;
    Size<int>               m_text_size  = {0, 0};
    int                     m_index      = 0;
    int                     m_running_x  = 0;

public:
    TextPainter();

   ~TextPainter();

    bool isGood() const;

    /* Recalculate text flow with the given wrap_mode..
     * The width parameter is used with multi-line modes
     * to determine the wrap point. */
    void reflow(const std::string &text, Font* font, TextWrap wrap_mode, int width);

    /* As calculated by the reflow method. */
    int lineCount() const;

    /* Size of the image that can fit the text.
     * As calculated be the reflow method. */
    Size<int> textSize() const;

    /* Paint text onto image.
     * Use offset from that top-left corner. */
    void paint(Image* image, Point<int> offset = {0, 0});

    void inputUtf8(const std::string &text);

private:
    bool glyphFits(Font::Glyph* glyph);

    void addGlyph(Font::Glyph* glyph);

    void addLine(Font* font);

    void clear();

    void retreatToWordStart(int &i);
};

}//namespace

#endif//R64FX_TEXT_PAINTER_HPP