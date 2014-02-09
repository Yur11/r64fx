#include "Menu.h"
#include "MouseEvent.h"
#include "Window.h"
#include "RectPainter.h"

#include <iostream>

using namespace std;


namespace r64fx{
   
    
 
template<typename T> T max(T a, T b)
{
    return a > b ? a : b;
}
 

    
Menu::Menu(Font* font, Widget* parent)
:VerticalContainer(parent)
,_font(font)
{
}


void Menu::appendAction(Action* act)
{
    appendWidget(new ActionWidget(act, _font));
}


void Menu::render()
{
    RectPainter::prepare();
    
    RectPainter::setTexture(RectPainter::plainTexture());
    RectPainter::setTexCoords(0.0, 0.0, 1.0, 1.0);
    
    RectPainter::setColor(0.0, 0.01, 0.0, 0.85);
    
    RectPainter::setCoords(0.0, 0.0, width(), height());
    RectPainter::render();
    
    render_children();
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