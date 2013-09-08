#ifndef R64FX_GUI_MOUSE_H
#define R64FX_GUI_MOUSE_H

namespace r64fx{
    
struct Mouse{
    struct Button{
        static const unsigned int None   = 0;
        static const unsigned int Left   = 1;
        static const unsigned int Middle = 1<<1;
        static const unsigned int Right  = 1<<2;
        static const unsigned int WheelUp = 1<<3;
        static const unsigned int WheelDown = 1<<4;
    };
};
    
}//namespace r64fx

#endif//R64FX_GUI_MOUSE_H
