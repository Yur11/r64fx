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

    inline int nextIndex() const { return index() + text().size(); }

    inline std::string text() const { return m_glyph->text(); }

    inline int textSize() const { return m_glyph->text().size(); }

    inline int advance() const { return m_glyph->advance(); }
};


class GlyphLine{
    int m_y;
    int m_begin;   //Index of the first glyph in line.
    int m_end;     //Index past the last glyph in line,
    int m_x_offset = 0;

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

    inline int glyphCount() const { return end() - begin(); }

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

    inline void setLine(int line) { m_line = line; }

    inline int line() const { return m_line; }

    inline void setColumn(int column) { m_column = column; }

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
    TextWrap                m_text_wrap           = TextWrap::None;
    TextAlignment           m_text_alignment      = TextAlignment::Left;
    int                     m_reflow_width        = 100;
    WhitespaceCleanup       m_whitespace_cleanup  = WhitespaceCleanup::FrontAndBack;

    std::vector<GlyphEntry> m_glyphs;
    std::vector<GlyphLine>  m_lines;
    Size<int>               m_text_size  = {0, 0};

    int                     m_index      = 0;
    int                     m_running_x  = 0;

    TextCursorPosition      m_cursor_position = {0, 0};
    TextCursorPosition      m_selection_start = {0, 0};
    TextCursorPosition      m_selection_end   = {0, 0};

    std::vector<Rect<int>>  m_selection_rects;
        //A list of rectangles that comprise selection background.

    int                     m_preferred_cursor_column = 0;
        //For moving cursor up and down.

public:
    TextPainter();

    ~TextPainter();

    /* Must be set before doing anything else! */
    Font* font = nullptr;

    void setTextWrap(TextWrap text_wrap);

    TextWrap textWrap() const;

    void setTextAlignment(TextAlignment text_alignment);

    TextAlignment textAlignment() const;

    void setReflowWidth(int width);

    int reflowWidth() const;

    void setWhitespaceCleanupPolicy(WhitespaceCleanup policy);

    WhitespaceCleanup whitespaceCleanupPolicy() const;

    /* Recalculate text flow with the given wrap_mode..
     * The width parameter is used with multi-line modes
     * to determine the wrap point. */
    void reflow(const std::string &text);

    void reallign();

    /* Update selection geometry data based on selection start and end positions. */
    void updateSelection();

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
    TextCursorPosition findCursorPosition(Point<int> p);

    Point<int> findCursorCoords(TextCursorPosition tcp);

    /* Return cursor position moved some number glyphs forwards or backwards.
     * Positive nglyphs values implies forward cursor movement. */
    TextCursorPosition movedBy(TextCursorPosition tcp, int nglyphs);

    /* Find TextCursorPosition based on text index. */
    TextCursorPosition textIndex2CursorPosition(int text_index);

    /* Find text index based on cursor position. */
    int cursorPosition2TextIndex(TextCursorPosition tcp);

    /* Find index of the glyph that is located after the cursor. */
    int glyphIndex(TextCursorPosition tcp) const;

    /* Find selection index range in text. */
    void findRangeTextIndices(int &begin, int &end, TextCursorPosition begin_tcp, TextCursorPosition end_tcp);

    void setCursorPosition(TextCursorPosition tcp);

    TextCursorPosition cursorPosition() const;

    void setSelectionStart(TextCursorPosition tcp);

    TextCursorPosition selectionStart() const;

    void setSelectionEnd(TextCursorPosition tcp);

    TextCursorPosition selectionEnd() const;

    bool hasSelection() const;

    void moveCursorUp();

    void moveCursorDown();

    void moveCursorLeft();

    void moveCursorRight();

    void homeCursor();

    void endCursor();

    void deleteAfterCursor(std::string &text);

    void deleteBeforeCursor(std::string &text);

    void inputUtf8(const std::string &utf8, std::string &text);

    void deleteSelection(std::string &text);

private:
    bool glyphFits(Font::Glyph* glyph);

    void addGlyph(Font::Glyph* glyph);

    void addLine();

    void clear();

    void retreatToWordStart(int &i);
};

}//namespace

#endif//R64FX_TEXT_PAINTER_HPP