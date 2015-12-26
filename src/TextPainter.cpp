#include "TextPainter.hpp"
#include "Font.hpp"
#include "StringUtils.hpp"
#include "ImageUtils.hpp"

#include <vector>
#include <iostream>

using namespace std;

namespace r64fx{

namespace{

struct GlyphEntry{
    Font::Glyph* glyph;
    int x;     //Origin x coord.
    int index; //Index in text.
    unsigned int flags = 0;

    GlyphEntry(Font::Glyph* glyph, int x, int index)
    : glyph(glyph)
    , x(x)
    , index(index)
    {}
};


struct Line{
    int y;
    int first_glyph;
    int num_glyphs;

    Line(int y, int first_glyph)
    : y(y)
    , first_glyph(first_glyph)
    , num_glyphs(0)
    {}
};


struct LineIndex{
    vector<Line> lines;
    vector<GlyphEntry> glyphs;
    int width;
};

}//namespace

#define m_line_index ((LineIndex*)m)


TextPainter::TextPainter()
{
    m = new LineIndex;
}


TextPainter::~TextPainter()
{
    delete m_line_index;
}


bool TextPainter::isGood() const
{
    return (image != nullptr) && (text != nullptr) && (font != nullptr);
}


void TextPainter::reflow(TextWrap wrap_mode, int width)
{
    auto &lines  = m_line_index->lines;
    auto &glyphs = m_line_index->glyphs;

    lines.clear();
    glyphs.clear();

    if(text->empty())
        return;

    m_line_index->width = width;

    lines.push_back(
        Line(font->ascender() + lines.size() * font->height(), 0)
    );

    int index = 0;
    int running_x = 0;
    for(;;)
    {
        int nbytes = next_utf8(*text, index);
        if(nbytes < 0)
        {
            cerr << "TextPainter: Error reading utf8!\n";
            return;
        }

        int next_index = index + nbytes;
        if(next_index > (int)text->size())
        {
            cerr << "TextPainter: String size mismatch!\n";
            return;
        }

        auto char_text = text->substr(index, nbytes);
        if(char_text == "\n")
        {
            if(wrap_mode != TextWrap::None)
            {
                lines.push_back(
                    Line(font->ascender() + lines.size() * font->height(), glyphs.size())
                );
                running_x = 0;
            }
        }
        else
        {
            Font::Glyph* glyph = nullptr;

            glyph = font->fetchGlyph(char_text);
            if(!glyph)
            {
                cerr << "TextPainter: Failed to fetch glyph!\n";
                break;
            }

            if(wrap_mode == TextWrap::None || (running_x + glyph->advance()) < width)
            {
                glyphs.push_back(
                    GlyphEntry(glyph, running_x, index)
                );
                lines.back().num_glyphs++;
            }
            else
            {
                lines.push_back(
                    Line(font->ascender() + lines.size() * font->height(), glyphs.size())
                );
                running_x = 0;

                glyphs.push_back(
                    GlyphEntry(glyph, running_x, index)
                );
                lines.back().num_glyphs++;
            }
            running_x += glyph->advance();
        }

        index = next_index;
        if(index >= (int)text->size())
            break;
    }
}


int TextPainter::lineCount() const
{
    return m_line_index->lines.size();
}


Size<int> TextPainter::textSize() const
{
    return{
        m_line_index->width, lineCount() * font->height()
    };
}


void TextPainter::paint(Image* image, Point<int> offset)
{
    for(auto &line : m_line_index->lines)
    {
        for(int i=0; i<line.num_glyphs; i++)
        {
            GlyphEntry &ge = m_line_index->glyphs[i+line.first_glyph];
            Font::Glyph* glyph = ge.glyph;
            if(glyph->image()->isGood())
            {
                implant(image, {
                    offset.x() + ge.x + glyph->bearing_x(),
                    offset.y() + line.y - glyph->bearing_y()
                }, glyph->image());
            }
        }
    }
}


void TextPainter::inputUtf8(const std::string &text)
{

}

}//namespace r64fx