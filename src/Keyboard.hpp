#ifndef R64FX_KEYBOARD_HPP
#define R64FX_KEYBOARD_HPP

#ifdef R64FX_USE_SDL2
#include "KeyboardSDL2.hpp"
#endif //R64FX_USE_SDL2

namespace r64fx{

namespace Keyboard{
namespace Key{

const char* toString(int key);

}//namespace Key
}//namespace Keyboard

}//namespace r64fx

#endif//R64FX_GUI_KEYBOARD_HPP