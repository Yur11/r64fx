#ifndef R64FX_GUI_TEXT_INPUT_EVENT_H
#define R64FX_GUI_TEXT_INPUT_EVENT_H

#include "Event.hpp"
#include <string>

namespace r64fx{
    
struct TextInputEvent : public Event{
    std::string text;
    
    TextInputEvent(std::string text = "")
    : text(text)
    {}
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXT_INPUT_EVENT_H