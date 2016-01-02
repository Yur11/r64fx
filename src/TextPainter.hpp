#ifndef R64FX_TEXT_PAINTER_HPP
#define R64FX_TEXT_PAINTER_HPP

#include "Color.hpp"
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

    inline int advance() const { return m_glyph->advance(); }
};


class GlyphLine{
    int m_y;
    int m_begin;   //Index of the first glyph in line.
    int m_end;     //Index past the last glyph in line,
    int m_x_offset = 0;
    int m_width = 0;

public:
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

    inline void setXOffset(int x_offset) { m_x_offset = x_offset; }

    inline int xOffset() const { return m_x_offset; }

    inline void setWidth(int width) { m_width = width; }

    inline int width() const { return m_width; }

    inline bool isEmpty() const { return m_begin == m_end; }
};


class TextCursorPosition{
    int m_line;
    int m_column;

public:
    TextCursorPosition(int line = 0, int column = 0)
    : m_line(line)
    , m_column(column)
    {}

    inline int line() const { return m_line; }

    inline int column() const { return m_column; }
};


inline bool operator<(TextCursorPosition a, TextCursorPosition b)
{
    return a.line() < b.line() || (a.line() == b.line() && a.column() < b.column());
}


inline bool operator>(TextCursorPosition a, TextCursorPosition b)
{
    return a.line() > b.line() || (a.line() == b.line() && a.column() > b.column());
}


inline bool operator==(TextCursorPosition a, TextCursorPosition b)
{
    return a.line() == b.line() && a.column() == b.column();
}


inline bool operator!=(TextCursorPosition a, TextCursorPosition b)
{
    return !operator==(a, b);
}


inline bool operator<=(TextCursorPosition a, TextCursorPosition b)
{
    return a < b || a == b;
}


inline bool operator>=(TextCursorPosition a, TextCursorPosition b)
{
    return a > b || a == b;
}


class TextPainter{
    std::vector<GlyphEntry> m_glyphs;
    std::vector<GlyphLine>  m_lines;
    Size<int>               m_text_size  = {0, 0};
    int                     m_index      = 0;
    int                     m_running_x  = 0;
    TextCursorPosition      m_selection_start;
    TextCursorPosition      m_selection_end;
    std::vector<Rect<int>>  m_selection_rects;      //A list of rectangles that comprise selection background.
    int                     m_selected_glyph_begin; //Index of the first selected glyph
    int                     m_selected_glyph_end;   //Index of the last selected glyph + 1

public:
    TextPainter();

   ~TextPainter();

    bool isGood() const;

    /* Recalculate text flow with the given wrap_mode..
     * The width parameter is used with multi-line modes
     * to determine the wrap point. */
    void reflow(const std::string &text, Font* font, TextWrap::Mode wrap_mode, int width);

    void reallign(TextAlign::Mode alignment);

    /* Update selection geometry data based on selection start and end positions. */
    void updateSelection(Font* font);

    /* As calculated by the reflow method. */
    int lineCount() const;

    /* Size of the image that can fit the text.
     * As calculated be the reflow method. */
    Size<int> textSize() const;

    /* Paint text onto image.
     * Use an offset from that top-left corner. */
    void paint(Image* image, Point<int> offset = {0, 0});

    void paintText(Image* image, Color<unsigned char> fg, Color<unsigned char> bg, Point<int> offset = {0, 0});

    void paintSelectionBackground(Image* image, Color<unsigned char> color, Point<int> offset);

    /* Find text cursor position based on a point within Rect{{0, 0}, textSize()}. */
    TextCursorPosition findCursorPosition(Point<int> p, Font* font);

    Point<int> findCursorPosition(TextCursorPosition tcp, Font* font);

    inline void setSelectionStart(TextCursorPosition pos)
    {
        m_selection_start = pos;
    }

    inline TextCursorPosition selectionStart() const
    {
        return m_selection_start;
    }

    inline void setSelectionEnd(TextCursorPosition pos)
    {
        m_selection_end = pos;
    }

    inline TextCursorPosition selectionEnd() const
    {
        return m_selection_end;
    }

    inline bool hasSelection() const
    {
        return selectionStart() != selectionEnd();
    }

private:
    bool glyphFits(Font::Glyph* glyph);

    void addGlyph(Font::Glyph* glyph);

    void addLine(Font* font);

    void clear();

    void retreatToWordStart(int &i);
};

}//namespace

#endif//R64FX_TEXT_PAINTER_HPP