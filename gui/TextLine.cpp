#include "TextLine.h"
#include <iostream>

using namespace std;

namespace r64fx{

void TextLine::render()
{
    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    _font->enable();
    _font->setColor(textColor());
    
    _font->setPenX(paddingLeft());
    _font->setPenY(paddingBottom() + _font->descender());

    _font->render(text.stdstr);
    
    Painter::enable();
    
    gl::Disable(GL_BLEND);
}


void TextLine::update()
{
    float width = _font->lineAdvance(text.stdstr) + paddingLeft() + paddingRight();
    float height = _font->ascender() + _font->descender() + paddingTop() + paddingBottom();
    
    if(width > _max_width) width = _max_width;
    if(width < _min_width) width = _min_width;
    
    Widget::resize(width, height);
}
    
}//namespace r64fx