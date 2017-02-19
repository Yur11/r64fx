#ifndef R64FX_KEYBOARD_HPP
#define R64FX_KEYBOARD_HPP

#if defined R64FX_USE_X11
#include "KeyboardX11.hpp"
#endif

#include <string>

namespace r64fx{

namespace Keyboard{
namespace Key{
    const char* toString(int key);

}//namespace Key
}//namespace Keyboard


}//namespace r64fx

#endif//R64FX_GUI_KEYBOARD_HPP