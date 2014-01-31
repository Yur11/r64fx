#include "Menu.h"
#include "MouseEvent.h"
#include "Window.h"

#include <iostream>

using namespace std;


namespace r64fx{
   
    
 
template<typename T> T max(T a, T b)
{
    return a > b ? a : b;
}
 

struct ActionWidget : public Widget, public Padding{
    Action* action;
    Font* font;
    bool is_highlighted = false;
    
    ActionWidget(Action* act, Font* font, Widget* parent = nullptr) 
    : Widget(parent)
    , action(act)
    , font(font)
    {
        setPaddingTop(5.0);
        setPaddingBottom(5.0);
    }
    
    virtual void render(RenderingContextId_t context_id)
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
    
    virtual void update()
    {
        float new_height = (font->ascender() + font->descender());
        setWidth(paddingLeft() + font->lineAdvance(action->name().stdstr) + new_height + 10 + paddingRight());
        setHeight(paddingTop() + new_height + paddingBottom());
        action->setIconSize(Size<float>(new_height * 1.5, new_height * 1.5));
    }
};
    
    
Menu::Menu(Font* font, Widget* parent)
:VerticalContainer(parent)
,_font(font)
{
}


void Menu::appendAction(Action* act)
{
    appendWidget(new ActionWidget(act, _font));
}


void Menu::render(RenderingContextId_t context_id)
{
//     glColor3f(0.0, 0.01, 0.0);
//     glBegin(GL_POLYGON);
//         glVertex2f(0.0, 0.0);
//         glVertex2f(width(), 0.0);
//         glVertex2f(width(), height());
//         glVertex2f(0.0, height());
//     glEnd();
    
    render_children(context_id);
}


void Menu::mousePressEvent(MouseEvent* event)
{
}


void Menu::mouseReleaseEvent(MouseEvent* event)
{
    if(event->buttons() | Mouse::Button::Left)
    {
        auto widget = childAt(event->position());
        if(widget)
        {
            widget->to<ActionWidget*>()->action->trigger();
            event->originWindow()->closeAllOverlayMenus();
        }
    }
}


void Menu::mouseMoveEvent(MouseEvent* event)
{
    auto widget = childAt(event->position());
    if(widget)
    {
        if(widget != highlighted_widget)
        {
            if(highlighted_widget)
            {
                highlighted_widget->to<ActionWidget*>()->is_highlighted = false;
            }
            highlighted_widget = widget;
            highlighted_widget->to<ActionWidget*>()->is_highlighted = true;
        }
    }
    else
    {
        if(highlighted_widget)
        {
            highlighted_widget->to<ActionWidget*>()->is_highlighted = false;
        }
        highlighted_widget = nullptr;
    }
}
    
}//namespace r64fx