#include "Window.hpp"

#if defined R64FX_USE_X11
#include "WindowX11.cpp"
#define WindowImpl WindowX11
#else
#error No window implementation defined!
#endif
