#include "TextLine.h"
#include "Error.h"
#include <iostream>

using namespace std;

namespace r64fx{

void TextLine::render()
{
    glEnable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_FOR_GL_ERRORS;
    
    _font->prepare();
    _font->setRGBA(
        textColor().red(),
        textColor().green(),
        textColor().blue(),
        textColor().alpha()
    );
    
    _font->setPenX(paddingLeft());
    _font->setPenY(paddingBottom() + _font->descender());

    _font->render(text.stdstr);
    
    glUseProgram(0);
    CHECK_FOR_GL_ERRORS;
    
    glDisable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
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