#include "TextPainter.hpp"
#include "Font.hpp"
#include "StringUtils.hpp"
#include "ImageUtils.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

namespace r64fx{

TextPainter::TextPainter()
{
}


TextPainter::~TextPainter()
{
}


void TextPainter::reflow(const std::string &text, Font* font, TextWrap::Mode wrap_mode, int width)
{
    if(text.empty())
        return;

    m_text_size.setWidth(width);

    clear();
    addLine(font);

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
            if(wrap_mode != TextWrap::None)
            {
                addLine(font);
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

            if(wrap_mode == TextWrap::None || wrap_mode == TextWrap::Newline || glyphFits(glyph))
            {
                addGlyph(glyph);
            }
            else
            {
                if(wrap_mode == TextWrap::Word)
                {
                    if(whitespace_count > 0)
                    {
                        auto &line = m_lines.back();
                        int i = m_glyphs.size() - 1;
                        retreatToWordStart(i);
                        line.setEnd(i);
                        addLine(font);
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
                else if(wrap_mode == TextWrap::Anywhere)
                {
                    addLine(font);
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
        for(int i=line.begin();  i != line.end()  && m_glyphs[i].text() == " ";  line.setBegin(++i));
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


void TextPainter::reallign(TextAlign::Mode alignment)
{
    for(auto &line : m_lines)
    {
        if(alignment == TextAlign::Left)
        {
            line.setXOffset(0);
        }
        else if(alignment == TextAlign::Right)
        {
            line.setXOffset(m_text_size.width() - line_width(line, m_glyphs));
        }
        else
        {
            line.setXOffset((m_text_size.width() - line_width(line, m_glyphs)) / 2);
        }
    }
}


void TextPainter::updateSelection(Font* font)
{
    m_selection_rects.clear();
    m_selected_glyph_begin = m_selected_glyph_end = 0;

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


TextCursorPosition TextPainter::findCursorPosition(Point<int> p, Font* font)
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


Point<int> TextPainter::findCursorPosition(TextCursorPosition tcp, Font* font)
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


void TextPainter::addLine(Font* font)
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