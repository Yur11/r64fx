#include "TextPainter.hpp"
#include "Font.hpp"
#include "StringUtils.hpp"
#include "ImageUtils.hpp"

#include <algorithm>
#include <limits>
#include <iostream>

using namespace std;

namespace r64fx{

TextPainter::TextPainter()
{
    clear();
}


TextPainter::~TextPainter()
{
}


void TextPainter::setTextWrap(TextWrap text_wrap)
{
    m_text_wrap = text_wrap;
}


TextWrap TextPainter::textWrap() const
{
    return m_text_wrap;
}


void TextPainter::setTextAlignment(TextAlignment text_alignment)
{
    m_text_alignment = text_alignment;
}


TextAlignment TextPainter::textAlignment() const
{
    return m_text_alignment;
}


void TextPainter::setReflowWidth(int width)
{
    m_reflow_width = width;
}


int TextPainter::reflowWidth() const
{
    return m_reflow_width;
}


void TextPainter::setWhitespaceCleanupPolicy(WhitespaceCleanup policy)
{
    m_whitespace_cleanup = policy;
}


WhitespaceCleanup TextPainter::whitespaceCleanupPolicy() const
{
    return m_whitespace_cleanup;
}


void TextPainter::setKeyboardSelectionMode(bool on)
{
    m_keyboard_selection_mode = on;
}

bool TextPainter::inKeyboardSelectionMode() const
{
    return m_keyboard_selection_mode;
}


void TextPainter::insertText(const std::string &text)
{
    int nglyphs = insertGlyphs(text);
    if(nglyphs > 0)
    {
        reflow();
        m_cursor_position = movedBy(m_cursor_position, nglyphs);
        if(lineStartsWithNewline(m_cursor_position.line()) && m_cursor_position.column() == 0)
        {
            m_cursor_position.setColumn(1);
        }
    }
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::reflow()
{
    m_text_size.setWidth(reflowWidth());
    clearLines();

    auto running_x = 0;
    for(int g=0; g<(int)m_glyphs.size(); g++)
    {
        auto &glyph = m_glyphs[g];
        int next_x = running_x + glyph.advance();
        if(textWrap() == TextWrap::None)
        {
            //Do nothing!
        }
        else if(textWrap() == TextWrap::Newline)
        {
            if(glyph.text() == "\n")
            {
                m_lines.push_back(GlyphLine(g, g));
                next_x = 0;
            }
        }
        else
        {
            if(next_x > m_reflow_width)
            {
                if(textWrap() == TextWrap::Word)
                {
                    auto &line = m_lines.back();

                    int new_g = g;
                    retreatToWordStart(new_g);
                    if(new_g > line.begin())
                    {
                        g = new_g;
                        line.setEnd(new_g);
                    }
                }
                else if(textWrap() == TextWrap::Token)
                {

                }

                m_lines.push_back(GlyphLine(g, g));
                next_x = 0;
            }
            else if(glyph.text() == "\n")
            {
                m_lines.push_back(GlyphLine(g, g));
                next_x = 0;
            }
        }

        auto &line = m_lines.back();
        line.setEnd(line.end() + 1);
        running_x = next_x;
    }//for

    m_text_size.setHeight(m_lines.size() * font->height());
}


int line_width(const GlyphLine &line, const std::vector<GlyphEntry> &glyphs)
{
    int width = 0;
    for(int i=line.begin(); i<line.end(); i++)
    {
        width += glyphs[i].glyph()->advance();
    }
    return width;
}


void TextPainter::reallign()
{
    for(auto &line : m_lines)
    {
        if(textAlignment() == TextAlignment::Left)
        {
            line.setXOffset(0);
        }
        else if(textAlignment() == TextAlignment::Right)
        {
            line.setXOffset(m_text_size.width() - line_width(line, m_glyphs));
        }
        else
        {
            line.setXOffset((m_text_size.width() - line_width(line, m_glyphs)) / 2);
        }
    }
}


void TextPainter::updateSelection()
{
    m_selection_rects.clear();

    TextCursorPosition a = m_selection_start;
    TextCursorPosition b = m_selection_end;

    if(a > b)
        swap(a, b);

    if(a.line() == b.line())
    {
        auto &line = m_lines[a.line()];

        if(a.column() == b.column())
        {
            /*No selection! Do nothing!*/
        }
        else
        {
            /*Selection on one line. Draw one rect.*/
            int x = line.xOffset();
            int i=0;
            while(i < a.column())
            {
                x += m_glyphs[i + line.begin()].advance();
                i++;
            }

            int width = 0;
            while(i < b.column())
            {
                width += m_glyphs[i + line.begin()].advance();
                i++;
            }

            Rect<int> rect(x, a.line()*font->height(), width, font->height());
            m_selection_rects.push_back(rect);
        }
    }
    else
    {
        /* First line selection. */
        {
            auto line = m_lines[a.line()];
            int x = line.xOffset();
            int w = 0;
            int i = line.begin();
            while((i - line.begin()) < a.column())
            {
                x += m_glyphs[i].advance();
                i++;
            }
            while(i < line.end())
            {
                w += m_glyphs[i].advance();
                i++;
            }

            Rect<int> rect(x, a.line()*font->height(), w, font->height());
            m_selection_rects.push_back(rect);
        }

        if((b.line() - a.line()) > 1)
        {
            for(auto l=a.line()+1; l<=b.line()-1; l++)
            {
                auto line = m_lines[l];
                if(line.isEmpty())
                    continue;

                int w = 0;
                for(auto i=line.begin(); i<line.end(); i++)
                {
                    w += m_glyphs[i].advance();
                }
                Rect<int> rect(line.xOffset(), l*font->height(), w, font->height());
                m_selection_rects.push_back(rect);
            }
        }

        /* Last line selection. */
        {
            auto line = m_lines[b.line()];
            int w = 0;
            int i = line.begin();
            while(i < (line.begin() + b.column()))
            {
                w += m_glyphs[i].advance();
                i++;
            }
            Rect<int> rect(line.xOffset(), b.line()*font->height(), w, font->height());
            m_selection_rects.push_back(rect);
        }
    }
}


int TextPainter::lineCount() const
{
    return m_lines.size();
}


Size<int> TextPainter::textSize() const
{
    return m_text_size;
}


void TextPainter::paint(Image* image, Point<int> offset)
{
    for(int l=0; l<(int)m_lines.size(); l++)
    {
        auto &line = m_lines[l];
        int x = line.xOffset();
        for(int i=line.begin(); i!=line.end(); i++)
        {
            GlyphEntry &ge = m_glyphs[i];
            Font::Glyph* glyph = ge.glyph();
            if(glyph->image()->isGood())
            {
                Point<int> p = {
                    offset.x() + glyph->bearing_x() + x,
                    offset.y() - glyph->bearing_y() + (l*font->height()) + font->ascender()
                };

                implant(image, p, glyph->image());
            }
            x += glyph->advance();
        }
    }
}


void TextPainter::paintText(Image* image, Color<unsigned char> fg, Color<unsigned char> bg, Point<int> offset)
{
    for(int l=0; l<(int)m_lines.size(); l++)
    {
        auto &line = m_lines[l];
        int x = line.xOffset();
        for(int i=line.begin(); i!=line.end(); i++)
        {
            GlyphEntry &ge = m_glyphs[i];
            Font::Glyph* glyph = ge.glyph();
            if(glyph->image()->isGood())
            {
                Point<int> p = {
                    offset.x() + glyph->bearing_x() + x,
                    offset.y() - glyph->bearing_y() + (l*font->height()) + font->ascender()
                };

                alpha_blend(image, p, fg, glyph->image());
            }
            x += glyph->advance();
        }
    }
}


void TextPainter::paintSelectionBackground(Image* image, Color<unsigned char> color, Point<int> offset)
{
    for(auto rect : m_selection_rects)
    {
        rect += offset;
        rect = intersection(rect, {0, 0, image->width(), image->height()});
        fill(image, color, rect);
    }
}


TextCursorPosition TextPainter::findCursorPosition(Point<int> p)
{
    if(m_glyphs.empty())
        return TextCursorPosition(0, 0);

    int i=0;
    if(p.y() > 0)
    {
        while(i < (int)m_lines.size())
        {
            int top     = font->height() * i;
            int bottom  = font->height() * (i + 1);

            if(p.y() > top && p.y() < bottom)
            {
                break;
            }

            i++;
        }
    }

    if(i >= (int)m_lines.size())
        i = m_lines.size() - 1;

    auto &line = m_lines[i];
    int x = line.xOffset();
    int n=line.begin();
    while(n<line.end())
    {
        auto &glyph = m_glyphs[n];
        if(p.x() < x + glyph.advance() / 2)
        {
            break;
        }
        x += glyph.advance();
        n++;
    }

    TextCursorPosition tcp(i, n - line.begin());
    if(tcp.column() == 0 && lineStartsWithNewline(tcp.line()))
    {
        tcp.setColumn(1);
    }

    return tcp;
}


Point<int> TextPainter::findCursorCoords(TextCursorPosition tcp)
{
    int y = -1;
    int x = -1;
    if(tcp.line() >= 0 && tcp.line() < (int)m_lines.size())
    {
        y = tcp.line() * font->height();
        auto &line = m_lines[tcp.line()];
        x = line.xOffset();
        int i=line.begin();
        while(i<line.end())
        {
            if(tcp.column() == (i - line.begin()))
            {
                break;
            }
            x += m_glyphs[i].advance();
            i++;
        }
    }

    return Point<int>(x, y);
}


TextCursorPosition TextPainter::movedBy(TextCursorPosition tcp, int nglyphs)
{
    if(nglyphs == 0)
        return tcp;

    if(nglyphs < 0) //Move backwards.
    {
        nglyphs = -nglyphs;
        while(nglyphs)
        {
            if(nglyphs <= tcp.column())
            {
                tcp.setColumn(tcp.column() - nglyphs);
                break;
            }
            else
            {
                nglyphs -= tcp.column();
                if(tcp.line() == 0)
                {
                    tcp.setColumn(0); //No more glyphs.
                    break;
                }
                else
                {
                    tcp.setLine(tcp.line() - 1);
                    tcp.setColumn(m_lines[tcp.line()].glyphCount() + 1);
                }
            }
        }
    }
    else //Move forwards.
    {
        while(nglyphs)
        {
            auto &line = m_lines[tcp.line()];
            int slack = line.glyphCount() - tcp.column();
            if(nglyphs <= slack)
            {
                tcp.setColumn(tcp.column() + nglyphs);
                break;
            }
            else
            {
                nglyphs -= slack + 1;
                if(tcp.line() == (int)m_lines.size() - 1)
                {
                    tcp.setColumn(line.glyphCount()); //No more glyphs.
                    break;
                }
                else
                {
                    tcp.setLine(tcp.line() + 1);
                    tcp.setColumn(0);
                }
            }
        }
    }

    return tcp;
}


int TextPainter::glyphIndex(TextCursorPosition tcp) const
{
    auto &line = m_lines[tcp.line()];
    return line.begin() + tcp.column();
}


GlyphEntry TextPainter::glyphAt(TextCursorPosition tcp) const
{
    return m_glyphs[glyphIndex(tcp)];
}


void TextPainter::setCursorPosition(TextCursorPosition tcp)
{
    m_cursor_position = tcp;
    m_preferred_cursor_column = tcp.column();
}


TextCursorPosition TextPainter::cursorPosition() const
{
    return m_cursor_position;
}


void TextPainter::setSelectionStart(TextCursorPosition tcp)
{
    m_selection_start = tcp;
}


TextCursorPosition TextPainter::selectionStart() const
{
    return m_selection_start;
}


void TextPainter::setSelectionEnd(TextCursorPosition tcp)
{
    m_selection_end = tcp;
}


TextCursorPosition TextPainter::selectionEnd() const
{
    return m_selection_end;
}


bool TextPainter::hasSelection() const
{
    return selectionStart() != selectionEnd();
}


void TextPainter::moveCursorUp()
{
    if(m_cursor_position.line() > 0)
    {
        moveCursorVertically(-1);
    }
}


void TextPainter::moveCursorDown()
{
    if(m_cursor_position.line() < int(m_lines.size() - 1))
    {
        moveCursorVertically(+1);
    }
}


void TextPainter::moveCursorVertically(int direction)
{
    int curr_line = m_cursor_position.line();
    int next_line = m_cursor_position.line() + direction;

    bool curr_got_newline = lineStartsWithNewline(curr_line);
    bool next_got_newline = lineStartsWithNewline(next_line);
    if(curr_got_newline)
    {
        if(!next_got_newline)
            m_preferred_cursor_column--;
    }
    else
    {
        if(next_got_newline)
            m_preferred_cursor_column++;
    }

    int next_col;
    if(m_preferred_cursor_column > m_lines[next_line].glyphCount())
    {
        next_col = m_lines[curr_line].glyphCount();
    }
    else
    {
        next_col = m_preferred_cursor_column;
    }

    m_cursor_position = TextCursorPosition(
        next_line, next_col
    );
}


void TextPainter::moveCursorLeft()
{
    int offset = -1;
    if(m_cursor_position.column() == 1 && lineStartsWithNewline(m_cursor_position.line()))
    {
        offset = -2;
    }
    m_cursor_position = movedBy(m_cursor_position, offset);
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::moveCursorRight()
{
    m_cursor_position = movedBy(m_cursor_position, +1);
    if(m_cursor_position.column() == 0 && lineStartsWithNewline(m_cursor_position.line()))
    {
        m_cursor_position.setColumn(
            m_cursor_position.column() + 1
        );
    }
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::homeCursor()
{
    if(lineStartsWithNewline(m_cursor_position.line()))
    {
        m_cursor_position.setColumn(1);
    }
    else
    {
        m_cursor_position.setColumn(0);
    }
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::endCursor()
{
    auto line = m_lines[m_cursor_position.line()];
    m_cursor_position.setColumn(line.glyphCount());
    m_preferred_cursor_column = std::numeric_limits<int>::max();
}


void TextPainter::deleteAfterCursor()
{
    if(hasSelection())
    {
        deleteSelection();
    }
    else
    {

    }
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::deleteBeforeCursor()
{
    if(hasSelection())
    {
        deleteSelection();
    }
    else
    {
        auto idx = glyphIndex(m_cursor_position);
        if(idx > 0)
        {
            idx--;
            m_glyphs.erase(m_glyphs.begin() + idx);
            reflow();
            m_cursor_position = movedBy(TextCursorPosition(0, 0), idx);
        }
    }
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::deleteSelection()
{
    if(m_selection_start > m_selection_end)
        swap(m_selection_start, m_selection_end);

    int begin_idx = glyphIndex(m_selection_start);
    int end_idx   = glyphIndex(m_selection_end);

    m_glyphs.erase(
        m_glyphs.begin() + begin_idx,
        m_glyphs.begin() + end_idx
    );

    m_selection_end = m_cursor_position = m_selection_start;
    reflow();
    updateSelection();
}


void TextPainter::clear()
{
    m_glyphs.clear();
    m_lines.clear();
    m_lines.push_back(
        GlyphLine(0, 0)
    );
    m_cursor_position = {0, 0};
}


int TextPainter::insertGlyphs(const std::string &text)
{
    int glyph_index = glyphIndex(m_cursor_position);
    int num_glyphs = 0;
    int text_index = 0;
    for(;;)
    {
        int nbytes = next_utf8(text, text_index);
        if(nbytes < 0)
        {
            cerr << "TextPainter: Error reading utf8!\n";
            return num_glyphs;
        }

        int next_index = text_index + nbytes;
        if(next_index > (int)text.size())
        {
            cerr << "TextPainter: String size mismatch!\n";
            return num_glyphs;
        }

        auto char_text = text.substr(text_index, nbytes);
        auto glyph = font->fetchGlyph(char_text);
        if(!glyph)
        {
            cerr << "TextPainter: Failed to fetch glyph!\n";
            break;
        }

        m_glyphs.insert(
            m_glyphs.begin() + glyph_index,
            GlyphEntry(glyph)
        );
        num_glyphs++;
        glyph_index++;

        text_index = next_index;
        if(text_index >= (int)text.size())
            break;
    }

    return num_glyphs;
}


void TextPainter::clearLines()
{
    m_lines.clear();
    m_lines.push_back(
        GlyphLine(0, 0)
    );
}


void TextPainter::retreatToWordStart(int &i)
{
    while( i > m_lines.back().begin() && m_glyphs[i].text() != " ")
        i--;
    i++;
}


bool TextPainter::lineStartsWithNewline(int l)
{
    auto &line = m_lines[l];
    return m_glyphs[line.begin()].isNewline();
}

}//namespace r64fx