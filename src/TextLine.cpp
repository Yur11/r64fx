#include "TextLine.hpp"
#include <iostream>

using namespace std;

namespace r64fx{

TextLine::TextLine(Utf8String text, Font* font, Widget* parent) 
: Widget(parent)
, _font(font)
, text_color(0.0, 0.0, 0.0)
, p(4)
, text(text)
{
    
    float tex_coords[8] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };

    p.bindBuffer();
    p.setTexCoords(tex_coords, 8);
    p.unbindBuffer();
}
    
    
void TextLine::render()
{
    auto br = projectedRect();
    
    _font->setPenX(br.left + paddingLeft());
    auto r = _font->calculateBoundingBox(text.stdstr);
    
    float pos[8] = {
        r.left,   r.top,
        r.right,  r.top,
        r.left,   r.bottom,
        r.right,  r.bottom
    };
    
    p.bindBuffer();
    p.setPositions(pos, 8);
    p.unbindBuffer();
    
    p.setColor(0.58, 0.79, 0.94, 1.0);
    
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();
    
    Painter::setTexturingMode(Painter::RedAsAlpha);
    _font->setColor(textColor());
    
    _font->setPenY(br.bottom - _font->descender());

    _font->render(text.stdstr);
    
    Painter::setTexturingMode(Painter::RGBA);
}


void TextLine::update()
{
    float width = _font->lineAdvance(text.stdstr) + paddingLeft() + paddingRight();
    float height = _font->ascender() + _font->descender() + paddingTop() + paddingBottom();
    
    if(width > _max_width) width = _max_width;
    if(width < _min_width) width = _min_width;
    
    Widget::setSize(width, height);
}
    
}//namespace r64fx