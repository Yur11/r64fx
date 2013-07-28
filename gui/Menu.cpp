#include "Menu.h"
#include "Font.h"


namespace r64fx{
    
 
template<typename T> T max(T a, T b)
{
    return a > b ? a : b;
}
 

struct ActionWidget : Widget{
    Action* action;
    
    ActionWidget(Action* act, Widget* parent = nullptr) 
    : Widget(parent)
    , action(act)
    {
    }
    
    virtual void render()
    {
        action->icon().render();
        glPushMatrix();
            glColor3f(0.9, 0.9, 0.9);
            glTranslatef(action->icon().size.w, 0.0, 0.0);
            Font::defaultFont()->render(action->name().stdstr);
        glPopMatrix();
    }
    
    virtual void update()
    {
        setWidth(100);
        setHeight(max(action->icon().size.h, Font::defaultFont()->ascender() + Font::defaultFont()->descender()));
    }
};
    
    
Menu::Menu(Widget* parent)
:VerticalContainer(parent)
{
}


void Menu::appendAction(Action* act)
{
    appendWidget(new ActionWidget(act));
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
    
}//namespace r64fx