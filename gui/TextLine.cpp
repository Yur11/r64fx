#include "TextLine.h"
#include <iostream>

using namespace std;

namespace r64fx{

void TextLine::render()
{      
    glPushMatrix();
        glTranslated(paddingLeft(), paddingBottom() + _font->descender(), 0.0);
        glColor(this->textColor());
        _font->render(text.stdstr.c_str());
    glPopMatrix();
}


void TextLine::update()
{
    float width = _font->estimatedTextWidth(text.c_str()) + paddingLeft() + paddingRight();
    float height = _font->ascender() + _font->descender() + paddingTop() + paddingBottom();
    
    if(width > _max_width) width = _max_width;
    if(width < _min_width) width = _min_width;
    
    Widget::resize(width, height);
}

    
}//namespace r64fx