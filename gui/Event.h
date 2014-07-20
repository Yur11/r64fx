#ifndef R64FX_GUI_EVENT_H
#define R64FX_GUI_EVENT_H

#include "Keyboard.h"

namespace r64fx{

class Window;
class View;
class Scene;
class HoverableWidget;

/** @brief Base class for keyboard, mouse and other events. 
 
    Event class containes information about event origins, 
    that can be usefull for the recipient object.
    The event is created by the window and is delivered
    down through the splitted views, to the scene 
    and further down the widget tree. As it does do 
    the view and scene pointers are set.
    
    If no widget is grabbing mouse or keyboard input i.e.
    the event is delivered in a normal way, the event mouse position
    value is transformed so that the event recipient allways gets
    the event position in it's own coordinate system.
    
    However is some widget is grabbing the input, event instance 
    is delivered directly to that widget and it's position is never
    transformed.
    
    If a Widget instance has used the event data and is not transmitting
    it down to it's children, it must set the has_been_handled flag to true.
    This will tell the obejcts up the event delivery tree that the event
    has found it's destination and should be left alone.
 */
class Event{
    friend class Window;
    friend class View;
    friend class Scene;
    
    Window* _origin_window = nullptr;
    View* _view = nullptr;
    Scene* _scene = nullptr;
    
    /** Orignal event position. */
    Point<float> _original_position = { 0.0, 0.0 };
    
    Point<float> _position = {0.0, 0.0};
    
    unsigned int _buttons = Mouse::Button::None;
    
    unsigned int _keyboard_modifiers = Keyboard::Modifier::None;
    
public:
    /** @brief Indicates whether the event has found it's destination. */
    bool has_been_handled = false;
    
    /** @brief Is set by the HoverableWidget on mouse move event. */
    HoverableWidget* hovered_widget = nullptr;
    
    Event(Point<float> position = Point<float>(), unsigned buttons = Mouse::Button::None, unsigned int keyboard_modifiers = Keyboard::Modifier::None) 
    : _original_position(position)
    , _position(position)
    , _buttons(buttons)
    , _keyboard_modifiers(keyboard_modifiers) 
    {}
    
    Event(float x, float y, unsigned buttons, unsigned int keyboard_modifiers = Keyboard::Modifier::None)
    : Event(Point<float>(x, y), buttons, keyboard_modifiers)
    {}
    
    /** @brief A window that originated the event. */
    inline Window* originWindow() const { return _origin_window; }
    
    /** @brief A leaf View instance that this event was dispatched to or nullptr. */
    inline View* view() const { return _view; }
    
    /** @brief A scene instance that this event was dispatched to or nullptr. */
    inline Scene* scene() const { return _scene; }

    /** @brief Mouse position transformed to the coordinate system of the destination View/Scene/Widget. 
     
        NOTE: This does not work when the destination widget is an event grabber. Only originalPosition()
              is valid in that case. Only relative mouse position changes are usually important 
              when the mouse input is being grabbed.
     */
    inline Point<float> position() const { return _position; }
    
    /** @brief Same as position().x */
    inline float x() const { return _position.x; }
    
    /** @brief Same as position().y */
    inline float y() const { return _position.y; }
    
    /** @brief Mouse position in the coordinate sytem of the window. */
    inline Point<float> originalPosition() const { return _original_position; }
    
    /** @brief Same as originalPosition().x */
    inline float original_x() const { return _original_position.x; }
    
    /** @brief Same as originalPosition().y */
    inline float original_y() const { return _original_position.y; }
    
    /** @brief Mouse buttons that are pressed or released.
     
        Use Mouse::Button structure extract values.
        For example: buttons() & Mouse::Button::Left
     */
    inline unsigned int buttons() const { return _buttons; }

     /** @brief Set the value that is returned by buttons().*/
    inline void setButtons(unsigned int buttons) { _buttons = buttons; }
    
    /** @brief Keyboard modifiers that were pressed during the event. 
     
        Use Keyboard::Modifier structure to extract values.
        For example: keyboardModifiers() & Keyboard::Modifier::Ctrl
     */
    inline int keyboardModifiers() { return _keyboard_modifiers; }
    
    /** @brief Set the value that is returned by keyboardModifiers().*/
    inline void setKeyboardModifiers(unsigned int keyboard_modifiers) { _keyboard_modifiers = keyboard_modifiers; }
    
    /** @brief Modify the event position.*/
    inline void operator+=(Point<float> other) { _position += other; }
    
    /** @brief Modify the event position.*/
    inline void operator-=(Point<float> other) { _position -= other; }
    
    /** @brief Modify the event position.*/
    inline void operator*=(Point<float> other) { _position *= other; }
    
    /** @brief Modify the event position.*/
    inline void operator*=(float coeff) { _position *= coeff; }
    
private:
    inline void setOriginWindow(Window* window) { _origin_window = window; }

    inline void setScene(Scene* scene) { _scene = scene; }
    
};

}//namespace r64fx

#endif//R64FX_GUI_EVENT_H