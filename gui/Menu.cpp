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
    
    virtual void render()
    {
        action->icon().render();
        glPushMatrix();
            if(is_highlighted)
                glColor3f(0.7, 0.9, 0.9);
            else
                glColor3f(0.9, 0.7, 0.7);
            glTranslatef(action->icon().size.w + 5, 2.0, 0.0);
            font->render(action->name().stdstr);
        glPopMatrix();
    }
    
    virtual void update()
    {
        float new_height = (font->ascender() + font->descender());
        setWidth(paddingLeft() + font->estimatedTextWidth(action->name().stdstr) + new_height + 10 + paddingRight());
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


void Menu::render()
{
    glColor3f(0.0, 0.01, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.0, 0.0);
        glVertex2f(width(), 0.0);
        glVertex2f(width(), height());
        glVertex2f(0.0, height());
    glEnd();
    
    render_children();
}


void Menu::mousePressEvent(MouseEvent* event)
{
//     cout << "press: " << event->x() << ", " << event->y() << "\n"; 
}


void Menu::mouseReleaseEvent(MouseEvent* event)
{
//     cout << "release: " << event->x() << ", " << event->y() << "\n";
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
//     cout << "move: " << event->x() << ", " << event->y() << "\n"; 
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