#include "TextPainter.hpp"
#include "Font.hpp"
#include "StringUtils.hpp"
#include "ImageUtils.hpp"

#include <iostream>

using namespace std;

namespace r64fx{


TextPainter::TextPainter()
{
}


TextPainter::~TextPainter()
{
}


void TextPainter::reflow(const std::string &text, Font* font, TextWrap wrap_mode, int width)
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

            if(wrap_mode == TextWrap::None || glyphFits(glyph))
            {
                addGlyph(glyph);
            }
            else
            {
                if(wrap_mode == TextWrap::Word)
                {
                    if(whitespace_count > 1)
                    {
                        auto &line = m_lines.back();
                        int i = m_glyphs.size();
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
    }

    m_text_size.setHeight(m_lines.size() * font->height());

    /* Remove extra spaces. */
    for(auto &line : m_lines)
    {
        for(int i=line.begin();  i != line.end()  && m_glyphs[i].text() == " ";  line.setBegin(++i));
        for(int i=line.end()-1;  i > line.begin() && m_glyphs[i].text() == " ";  line.setEnd(--i));
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
        int x = 0;
        for(int i=line.begin(); i!=line.end(); i++)
        {
            GlyphEntry &ge = m_glyphs[i];
            Font::Glyph* glyph = ge.glyph();
            if(glyph->image()->isGood())
            {
                implant(image, {
                    offset.x() + glyph->bearing_x() + x,
                    offset.y() - glyph->bearing_y() + line.y()
                }, glyph->image());
            }
            x += glyph->advance();
        }
    }
}


void TextPainter::inputUtf8(const std::string &text)
{

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
    while(i > m_lines.back().begin() && m_glyphs[i].text() != " ")
        i--;
}

}//namespace r64fx