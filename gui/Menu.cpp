#include "Menu.h"
#include "MouseEvent.h"
#include "Window.h"
#include "Painter.h"

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
, p(4)
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


void Menu::appendAction(Action* act)
{
    appendWidget(new ActionWidget(act, _font));
}


void Menu::update()
{
    VerticalContainer::update();
    
    const auto &r = rect();
    
//     float pos[8] = {
//         r.left, r.bottom,
//         r.right, r.bottom,
//         r.left, r.top,
//         r.right, r.top
//     };
    
    float pos[8] = {
        0.0, 0.0,
        r.width(), 0.0,
        0.0, r.height(),
        r.width(), r.height()
    };
    
    p.bindBuffer();
    p.setPositions(pos, 8);
    p.unbindBuffer();
}


void Menu::render()
{        
    Painter::useCurrent2dProjection();
    Painter::useNoTexture();
    Painter::setColor(0.0, 0.0, 0.0, 1.0);
    
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();
        
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