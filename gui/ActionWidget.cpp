#include "ActionWidget.h"
#ifdef DEBUG
#include <assert.h>
#include <iostream>
using namespace std;
#endif//DEBUG

namespace r64fx{
    
ActionWidget::ActionWidget(Action* act, Font* font, Widget* parent) 
: Widget(parent)
, action(act)
, font(font)
{
    setPaddingTop(5.0);
    setPaddingBottom(5.0);
}


void ActionWidget::render()
{
    gl::Enable(GL_BLEND);
    
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    icon.render();
    font->enable();
    font->useCurrent2dProjection();
    
    if(is_highlighted)
        font->setColor(0.7, 0.9, 0.9, 1.0);
    else
        font->setColor(0.9, 0.7, 0.7, 1.0);
    int x_offset = 4.0 + icon.size.w;
    while(!(x_offset / 2))
        x_offset++;
    font->setPenX(x_offset);
    font->setPenY(2.0);
        font->render(action->caption);
    
    font->disable();
    
    gl::Disable(GL_BLEND);
}


void ActionWidget::update()
{
    float new_height = (font->ascender() + font->descender());
    setWidth(paddingLeft() + font->lineAdvance(action->caption) + new_height + 10 + paddingRight());
    setHeight(paddingTop() + new_height + paddingBottom());
    
    if(icon.texture == nullptr)
    {
        auto tex = Texture::find(action->name);
        if(tex)
        {
#ifdef DEBUG
            assert(tex->kind() == Texture::Kind::Tex2D);
#endif//DEBUG
            icon.texture = (Texture2D*)tex;
        }
    }    
}

    
}//namespace r64fx