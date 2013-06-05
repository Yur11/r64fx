#include "menus.h"

namespace r64fx{
    
void HorizontalMenuSeparator::render()
{
    glColor(color());
    glBegin(GL_LINES);
        glVertex2f(0.0, 1.0);
        glVertex2f(width(), 1.0);
    glEnd();
}
    
    
void ContextMenu::render()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor(backgroundColor());
    glBegin(GL_POLYGON);
        glVertex2f(0.0, 0.0);
        glVertex2f(width(), 0.0);
        glVertex2f(width(), height());
        glVertex2f(0.0, height());
    glEnd();
    glDisable(GL_BLEND);
    
    render_children();
}
    
}//namespace r64fx
