#ifndef R64FX_GUI_MOUSE_EVENT_H
#define R64FX_GUI_MOUSE_EVENT_H

#include "Mouse.h"

namespace r64fx{

class WindowBase;
class SplittableView;
class Scene;
class Widget;


/** @brief Mouse movement and button event data structure. 
 
    Mouse events are usually created by the window and delivered down through the views, the scene
    and the widget tree.
 */
class MouseEvent{
    Point<float> _position;
    
    /** Orignal event position. */
    Point<float> _original_position;
    unsigned int _buttons;
   
public:
    MouseEvent(Point<float> position, unsigned int buttons = Mouse::Button::None)
    : _position(position)
    , _original_position(position)
    , _buttons(buttons)
    {}
    
    MouseEvent(float x = 0.0, float y = 0.0, unsigned int buttons = Mouse::Button::None)
    : MouseEvent(Point<float>(x, y), buttons)
    {}
    
    /** We can do feedback via mouse event objecs. */
    WindowBase* window = nullptr;
    SplittableView* view = nullptr;
    Scene* scene = nullptr;
    Widget* widget = nullptr;
    
    inline Point<float> position() const { return _position; }
    
    inline float x() const { return _position.x; }
    inline float y() const { return _position.y; }
    
    inline Point<float> originalPosition() const { return _original_position; }
    
    inline float original_x() const { return _original_position.x; }
    inline float original_y() const { return _original_position.y; }
    
    inline void setButtons(unsigned int buttons) { _buttons = buttons; }
    
    inline unsigned int buttons() const { return _buttons; }
    
    inline MouseEvent operator+(Point<float> other) { return MouseEvent(x() + other.x, y() + other.y, buttons()); }
    inline MouseEvent operator-(Point<float> other) { return MouseEvent(x() - other.x, y() - other.y, buttons()); }
    inline MouseEvent operator*(Point<float> other) { return MouseEvent(x() * other.x, y() * other.y, buttons()); }
    inline MouseEvent operator*(float coeff) { return MouseEvent(x() * coeff, y() * coeff, buttons()); }
    
    inline void operator+=(Point<float> other) { *this = *this + other; }
    inline void operator-=(Point<float> other) { *this = *this - other; }
    inline void operator*=(Point<float> other) { *this = *this * other; }
    inline void operator*=(float coeff) { *this = *this * coeff; }
};

}//namespace r64fx

#endif//R64FX_GUI_MOUSE_EVENT_H