#ifndef R64FX_GUI_DUMMY_H
#define R64FX_GUI_DUMMY_H

#include "Widget.h"

namespace r64fx{
    
class Menu;
    
/** @brief Just a dummy widget that draws a rectangle. */
class Dummy : public Widget{
    Point<float> _last_mouse_move;
    
public:
    Dummy(float width, float height, Widget* parent = nullptr) : Widget(parent) { resize(width, height); }
    
    virtual void render();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    static void initDebugMenu();
};
    
}//namespace r64fx

#endif//R64FX_GUI_DUMMY_H
