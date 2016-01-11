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
    Font::Glyph*   m_glyph;

public:
    GlyphEntry(Font::Glyph* glyph)
    : m_glyph(glyph)
    {}

    inline Font::Glyph* glyph() const { return m_glyph; }

    inline std::string text() const { return m_glyph->text(); }

    inline int textSize() const { return m_glyph->text().size(); }

    inline int advance() const { return m_glyph->advance(); }

    inline bool isNewline() const { return text() == "\n"; }
};


class GlyphLine{
    int m_begin;   //Index of the first glyph in line.
    int m_end;     //Index past the last glyph in line,
    int m_x_offset = 0;

public:
    GlyphLine(int begin, int end)
    : m_begin(begin)
    , m_end(end)
    {}

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


template<typename StreamT> StreamT &operator<<(StreamT &stream, TextCursorPosition tcp)
{
    stream << tcp.line() << ", " << tcp.column();
    return stream;
}


/* Helper class for rendering and editing text. */
class TextPainter{
    TextWrap                m_text_wrap           = TextWrap::None;
    TextAlignment           m_text_alignment      = TextAlignment::Left;
    int                     m_reflow_width        = 100;

    std::vector<GlyphEntry> m_glyphs;
    std::vector<GlyphLine>  m_lines;
    Size<int>               m_text_size  = {0, 0};

    TextCursorPosition      m_cursor_position = {0, 0};
    TextCursorPosition      m_selection_start = {0, 0};
    TextCursorPosition      m_selection_end   = {0, 0};

    /* A list of rectangles that comprise selection background. */
    std::vector<Rect<int>>  m_selection_rects;

    /* For moving cursor up and down. */
    int                     m_preferred_cursor_column = 0;


public:
    /* Must be set before doing anything else! */
    Font* font = nullptr;

    TextPainter();

    ~TextPainter();


    /* Configs. */
    void setTextWrap(TextWrap text_wrap);

    TextWrap textWrap() const;

    void setReflowWidth(int width);

    int reflowWidth() const;

    void setTextAlignment(TextAlignment text_alignment);

    TextAlignment textAlignment() const;


    /* Text input. */
    void insertText(const std::string &text);

private:
    int insertGlyphs(const std::string &text);

public:
    /* Reflow text using current reflow width. */
    void reflow();

    /* Allign text using current text alignment setting. */
    void reallign();

    /* Update selection geometry data based on selection start and end positions. */
    void updateSelection();


    /* As calculated by the reflow method. */
    int lineCount() const;

    /* Size of the rectangle that can fit the text.
     * As calculated be the reflow method. */
    Size<int> textSize() const;


    /* Rendering images. */
    void paint(Image* image, Point<int> offset = {0, 0});

    void paintText(Image* image, Color<unsigned char> fg, Color<unsigned char> bg, Point<int> offset = {0, 0});

    void paintSelectionBackground(Image* image, Color<unsigned char> color, Point<int> offset);


    /* Rendering text. */
    void getText(std::string &str);

    void getText(std::string &str, int a, int b);

    void getText(std::string &str, TextCursorPosition a, TextCursorPosition b);


    /* Conversions. */
    TextCursorPosition findCursorPosition(Point<int> p) const;

    Point<int> findCursorCoords(TextCursorPosition tcp) const;

    int cursorPositionToGlyphIndex(TextCursorPosition tcp) const;

    TextCursorPosition glyphIndexToCursorPosition(int index) const;


    /* Cursor movement. */
    void setCursorPosition(TextCursorPosition tcp);

    TextCursorPosition cursorPosition() const;

    void moveCursorUp();

    void moveCursorDown();

private:
    void moveCursorVertically(int direction);

public:

    void moveCursorLeft();

    void moveCursorRight();

    void homeCursor();

    void endCursor();


    /* Text selection. */
    void setSelectionStart(TextCursorPosition tcp);

    TextCursorPosition selectionStart() const;

    void setSelectionEnd(TextCursorPosition tcp);

    TextCursorPosition selectionEnd() const;

    void selectAll();

    void selectUp();

    void selectDown();

    void selectLeft();

    void selectRight();

    void homeSelection();

    void endSelection();

    void clearSelection();

    bool hasSelection() const;


    /* Text deletion. */
    void deleteAfterCursor();

    void deleteBeforeCursor();

    void deleteSelection();


    void clear();

private:
    void clearLines();

    void retreatToWordStart(int &i);

    bool lineStartsWithNewline(int l) const;


public:
    bool processTextInput(unsigned int key, const std::string &text, bool shift_down, bool ctrl_down);
};

}//namespace

#endif//R64FX_TEXT_PAINTER_HPP