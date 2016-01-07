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


void TextPainter::reflow(const std::string &text)
{
    if(text.empty())
        return;

    m_text_size.setWidth(reflowWidth());

    clear();
    addLine();

    int whitespace_count = 0;
    bool got_whitespace = false;
    for(;;)
    {
        int nbytes = next_utf8(text, m_index);
        if(nbytes < 0)
        {
            cerr << "TextPainter: Error reading utf8!\n";
            return;
        }

        int next_index = m_index + nbytes;
        if(next_index > (int)text.size())
        {
            cerr << "TextPainter: String size mismatch!\n";
            return;
        }

        auto char_text = text.substr(m_index, nbytes);
        if(char_text == "\n")
        {
            if(textWrap() != TextWrap::None)
            {
                addLine();
                whitespace_count = 0;
            }
        }
        else
        {
            if(char_text == " ")
            {
                if(!got_whitespace)
                    whitespace_count++;
                got_whitespace = true;
            }
            else
            {
                got_whitespace = false;
            }

            Font::Glyph* glyph = nullptr;

            glyph = font->fetchGlyph(char_text);
            if(!glyph)
            {
                cerr << "TextPainter: Failed to fetch glyph!\n";
                break;
            }

            if(textWrap() == TextWrap::None || textWrap() == TextWrap::Newline || glyphFits(glyph))
            {
                addGlyph(glyph);
            }
            else
            {
                if(textWrap() == TextWrap::Word)
                {
                    if(whitespace_count > 0)
                    {
                        auto &line = m_lines.back();
                        int i = m_glyphs.size() - 1;
                        retreatToWordStart(i);
                        line.setEnd(i);
                        addLine();
                        whitespace_count = 0;
                        m_lines.back().setBegin(i);
                        while(i < (int)m_glyphs.size())
                        {
                            auto &ge = m_glyphs[i];
                            m_running_x += ge.glyph()->advance();
                            i++;
                        }
                    }
                    addGlyph(glyph);
                }
                else if(textWrap() == TextWrap::Anywhere)
                {
                    addLine();
                    whitespace_count = 0;
                    addGlyph(glyph);
                }
                else
                {
                    addGlyph(glyph);
                }
            }
        }

        m_index = next_index;
        if(m_index >= (int)text.size())
            break;
    }//for

    m_text_size.setHeight(m_lines.size() * font->height());

    /* Remove extra spaces. */
    for(auto &line : m_lines)
    {
        if(m_whitespace_cleanup == WhitespaceCleanup::FrontAndBack || m_whitespace_cleanup == WhitespaceCleanup::Front)
            for(int i=line.begin();  i != line.end()  && m_glyphs[i].text() == " ";  line.setBegin(++i));

        if(m_whitespace_cleanup == WhitespaceCleanup::FrontAndBack || m_whitespace_cleanup == WhitespaceCleanup::Back)
            for(int i=line.end()-1;  i > line.begin() && m_glyphs[i].text() == " ";  line.setEnd(--i));
    }
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
    for(auto &line : m_lines)
    {
        int x = line.xOffset();
        for(int i=line.begin(); i!=line.end(); i++)
        {
            GlyphEntry &ge = m_glyphs[i];
            Font::Glyph* glyph = ge.glyph();
            if(glyph->image()->isGood())
            {
                Point<int> p = {
                    offset.x() + glyph->bearing_x() + x,
                    offset.y() - glyph->bearing_y() + line.y()
                };

                implant(image, p, glyph->image());
            }
            x += glyph->advance();
        }
    }
}


void TextPainter::paintText(Image* image, Color<unsigned char> fg, Color<unsigned char> bg, Point<int> offset)
{
    for(auto &line : m_lines)
    {
        int x = line.xOffset();
        for(int i=line.begin(); i!=line.end(); i++)
        {
            GlyphEntry &ge = m_glyphs[i];
            Font::Glyph* glyph = ge.glyph();
            if(glyph->image()->isGood())
            {
                Point<int> p = {
                    offset.x() + glyph->bearing_x() + x,
                    offset.y() - glyph->bearing_y() + line.y()
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

    return TextCursorPosition(i, n - line.begin());
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


TextCursorPosition TextPainter::textIndex2CursorPosition(int text_index)
{
    if(m_lines.empty())
        return TextCursorPosition(0, 0);

    for(int l=0; l<(int)m_lines.size(); l++)
    {
        auto &line = m_lines[l];
        for(int c=0; c<line.glyphCount(); c++)
        {
            auto glyph = m_glyphs[line.begin() + c];
            if(glyph.index() >= text_index)
                return TextCursorPosition(l, c);
        }
    }

    return TextCursorPosition(m_lines.size() - 1, m_lines.back().glyphCount());
}


int TextPainter::cursorPosition2TextIndex(TextCursorPosition tcp)
{
    if(tcp.line() < 0 || m_glyphs.empty())
        return 0;

    if(tcp.line() >= (int)m_lines.size())
        return m_glyphs.back().nextIndex();

    auto &line = m_lines[tcp.line()];
    if(tcp.column() < line.glyphCount())
    {
        return m_glyphs[line.begin() + tcp.column()].index();
    }

    return m_glyphs[line.end() - 1].nextIndex();
}


int TextPainter::glyphIndex(TextCursorPosition tcp) const
{
    auto &line = m_lines[tcp.line()];
    return line.begin() + tcp.column();
}


void TextPainter::findRangeTextIndices(int &begin, int &end, TextCursorPosition begin_tcp, TextCursorPosition end_tcp)
{
    begin = end = 0;
    int begin_glyph_idx = glyphIndex(begin_tcp);
    int end_glyph_idx   = glyphIndex(end_tcp);

    if(begin_glyph_idx > end_glyph_idx)
    {
        swap(begin_glyph_idx, end_glyph_idx);
    }

    if(begin_glyph_idx < 0)
    {
        begin_glyph_idx = 0;
    }

    if(end_glyph_idx >= (int)m_glyphs.size())
    {
        end_glyph_idx = m_glyphs.size() - 1;
        end += m_glyphs.back().textSize();
    }

    begin += m_glyphs[begin_glyph_idx].index();
    end   += m_glyphs[end_glyph_idx].index();
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
        int new_line = m_cursor_position.line() - 1;
        m_cursor_position = TextCursorPosition(
            new_line,
            min(m_preferred_cursor_column, m_lines[new_line].glyphCount())
        );
    }
}


void TextPainter::moveCursorDown()
{
    if(m_cursor_position.line() < int(m_lines.size() - 1))
    {
        int new_line = m_cursor_position.line() + 1;
        m_cursor_position = TextCursorPosition(
            new_line,
            min(m_preferred_cursor_column, m_lines[new_line].glyphCount())
        );
    }
}


void TextPainter::moveCursorLeft()
{
    m_cursor_position = movedBy(m_cursor_position, -1);
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::moveCursorRight()
{
    m_cursor_position = movedBy(m_cursor_position, +1);
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::homeCursor()
{
    m_cursor_position.setColumn(0);
    m_preferred_cursor_column = 0;
}


void TextPainter::endCursor()
{
    auto line = m_lines[m_cursor_position.line()];
    m_cursor_position.setColumn(line.glyphCount());
    m_preferred_cursor_column = std::numeric_limits<int>::max();
}


void TextPainter::deleteAfterCursor(std::string &text)
{
    if(hasSelection())
    {
        deleteSelection(text);
    }
    else
    {
//         auto new_cursor_position = movedBy(m_cursor_position, +1);
//         if(new_cursor_position > m_cursor_position)
//         {
//             int text_begin;
//             int text_end;
//             findRangeTextIndices(text_begin, text_end, new_cursor_position, m_cursor_position);
//             text.erase(text.begin() + text_begin, text.begin() + text_end);
//         }
    }
//     m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::deleteBeforeCursor(std::string &text)
{
    if(hasSelection())
    {
        deleteSelection(text);
        reflow(text);
        updateSelection();
    }
    else
    {
        auto new_cursor_position = movedBy(m_cursor_position, -1);
        if(new_cursor_position < m_cursor_position)
        {
            int text_begin = cursorPosition2TextIndex(new_cursor_position);
            int text_end   = cursorPosition2TextIndex(m_cursor_position);
//             findRangeTextIndices(text_begin, text_end, new_cursor_position, m_cursor_position);
            text.erase(text.begin() + text_begin, text.begin() + text_end);
            reflow(text);
            m_cursor_position = textIndex2CursorPosition(text_begin);
        }
    }
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::inputUtf8(const std::string &utf8, std::string &text)
{
    m_preferred_cursor_column = m_cursor_position.column();
}


void TextPainter::deleteSelection(std::string &text)
{
    int text_begin;
    int text_end;
    findRangeTextIndices(text_begin, text_end, m_selection_start, m_selection_end);
    text.erase(text.begin() + text_begin, text.begin() + text_end);
    m_selection_start =
    m_selection_end   =
    m_cursor_position =
    (m_selection_start < m_selection_end ? m_selection_start : m_selection_end);
}


bool TextPainter::glyphFits(Font::Glyph* glyph)
{
    return (m_running_x + glyph->advance()) < m_text_size.width();
}


void TextPainter::addGlyph(Font::Glyph* glyph)
{
    m_glyphs.push_back(
        GlyphEntry(glyph, m_index)
    );
    m_running_x += glyph->advance();
    m_lines.back().setEnd(
        m_lines.back().end() + 1
    );
}


void TextPainter::addLine()
{
    m_lines.push_back(
        GlyphLine(font->ascender() + m_lines.size() * font->height(), m_glyphs.size(), m_glyphs.size())
    );
    m_running_x = 0;
}


void TextPainter::clear()
{
    m_glyphs.clear();
    m_lines.clear();
    m_index = 0;
}


void TextPainter::retreatToWordStart(int &i)
{
    while( i > m_lines.back().begin() && m_glyphs[i].text() != " ")
        i--;
}

}//namespace r64fx