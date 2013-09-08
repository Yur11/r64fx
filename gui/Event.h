#ifndef R64FX_GUI_EVENT_H
#define R64FX_GUI_EVENT_H

#include "Keyboard.h"

namespace r64fx{

class WindowBase;
class View;
class Scene;

/** @brief Base class for keyboard, mouse and other events. 
 
    Event class containes information about event origins, 
    that can be usefull for the recipient object.
    As the event is created by the window and is delivered
    down through the splitted views, to the scene 
    and further down the widget tree it is filled with data.
    This data consits of:
    
        - A pointer to the window that orinated the event.
        
        - A pointer to the scene that recieved the event.
 */
class Event{
    friend class WindowBase;
    friend class View;
    friend class Scene;
    
    WindowBase* _origin_window = nullptr;
    View* _view = nullptr;
    Scene* _scene = nullptr;
    
    /** Orignal event position. */
    Point<float> _original_position = { 0.0, 0.0 };
    
    Point<float> _position = {0.0, 0.0};
    
    unsigned int _buttons;
    
    unsigned int _keyboard_modifiers = Keyboard::Modifier::None;
    
public:
    bool has_been_handled = false;
    
    Event(Point<float> position, unsigned buttons, unsigned int keyboard_modifiers = Keyboard::Modifier::None) 
    : _original_position(position)
    , _position(position)
    , _buttons(buttons)
    , _keyboard_modifiers(keyboard_modifiers) 
    {}
    
    Event(float x, float y, unsigned buttons, unsigned int keyboard_modifiers = Keyboard::Modifier::None)
    : Event(Point<float>(x, y), buttons, keyboard_modifiers)
    {}
    
    inline WindowBase* originWindow() const { return _origin_window; }
    
    inline View* view() const { return _view; }
    
    inline Scene* scene() const { return _scene; }

    inline Point<float> position() const { return _position; }
    
    inline float x() const { return _position.x; }
    inline float y() const { return _position.y; }
    
    inline Point<float> originalPosition() const { return _original_position; }
    
    inline float original_x() const { return _original_position.x; }
    inline float original_y() const { return _original_position.y; }
    
    inline void setButtons(unsigned int buttons) { _buttons = buttons; }
    
    inline unsigned int buttons() const { return _buttons; }
    
    inline int keyboardModifiers() { return _keyboard_modifiers; }
    
    inline void setKeyboardModifiers(unsigned int keyboard_modifiers) { _keyboard_modifiers = keyboard_modifiers; }
    
    inline void operator+=(Point<float> other) { _position += other; }
    inline void operator-=(Point<float> other) { _position -= other; }
    inline void operator*=(Point<float> other) { _position *= other; }
    inline void operator*=(float coeff) { _position *= coeff; }
    
private:
    inline void setOriginWindow(WindowBase* window) { _origin_window = window; }

    inline void setScene(Scene* scene) { _scene = scene; }
    
};

}//namespace r64fx

#endif//R64FX_GUI_EVENT_H