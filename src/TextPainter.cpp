#include "TextPainter.hpp"
#include <vector>

#include <iostream>

using namespace std;

namespace r64fx{

namespace{

struct LineIndex{
    struct Line{
        int y, left, right, index;
    };

    vector<LineIndex::Line> lines;

    Point<int> pen_position = {0, 0};
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


void TextPainter::putPen(Point<int> p)
{
    m_line_index->pen_position = p;
}


Point<int> TextPainter::penPosition() const
{
    return m_line_index->pen_position;
}


void TextPainter::inputUtf8(const char* bytes, int nbytes)
{
    string str(bytes, nbytes);
    cout << "--> " << str << "\n";
}


void TextPainter::deleteBeforeCursor()
{

}


void TextPainter::deleteAfterCursor()
{

}


void TextPainter::clear()
{

}


void TextPainter::homeCursor()
{

}


void TextPainter::endCursor()
{

}


void TextPainter::putCursor(int index)
{

}


void TextPainter::putCursor(Point<int> p)
{

}


int TextPainter::cursorIndex() const
{
    return 0;
}


void TextPainter::setCursorVisible(bool yes)
{

}


bool TextPainter::cursorVisible()
{
    return false;
}


void TextPainter::resetRect()
{

}


Rect<int> TextPainter::rectToUpdate()
{
    return {};
}

}//namespace r64fx