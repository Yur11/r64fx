#include "ActionWidget.h"

namespace r64fx{
    
ActionWidget::ActionWidget(Action* act, Font* font, Widget* parent) 
: Widget(parent)
, action(act)
, font(font)
{
    setPaddingTop(5.0);
    setPaddingBottom(5.0);
}


void ActionWidget::render(RenderingContextId_t context_id)
{
    glDisable(GL_TEXTURE_2D);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    action->icon().render(context_id);
    font->prepare();
    if(is_highlighted)
        font->setRGBA(0.7, 0.9, 0.9, 1.0);
    else
        font->setRGBA(0.9, 0.7, 0.7, 1.0);
    font->setPenX(action->icon().size.w + 5);
    font->setPenY(2.0);
    font->render(context_id, action->name().stdstr);
    
    glUseProgram(0);
    
    glDisable(GL_BLEND);
}


void ActionWidget::update()
{
    float new_height = (font->ascender() + font->descender());
    setWidth(paddingLeft() + font->lineAdvance(action->name().stdstr) + new_height + 10 + paddingRight());
    setHeight(paddingTop() + new_height + paddingBottom());
    action->setIconSize(Size<float>(new_height * 1.5, new_height * 1.5));
}

    
}//namespace r64fx