#ifndef R64FX_GUI_TEXT_INPUT_EVENT_H
#define R64FX_GUI_TEXT_INPUT_EVENT_H

#include <string>

namespace r64fx{
    
struct TextInputEvent : public Event{
    std::string text;
    
    TextInputEvent(Window* origin_window, std::string text, unsigned int keyboard_modifiers)
    : Event(origin_window, keyboard_modifiers)
    , text(text)
    {}
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXT_INPUT_EVENT_H