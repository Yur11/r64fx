#include "TextLine.h"
#include <iostream>

using namespace std;

namespace r64fx{

void TextLine::render(RenderingContextId_t context_id)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    _font->prepare();
    _font->setRGBA(
        textColor().r,
        textColor().g,
        textColor().b,
        textColor().a
    );
    
    _font->setPenX(paddingLeft());
    _font->setPenY(paddingBottom() + _font->descender());

    _font->render(context_id, text.stdstr);
    
    glUseProgram(0);
    
    glDisable(GL_BLEND);
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