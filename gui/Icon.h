#ifndef R64FX_GUI_ICON_H
#define R64FX_GUI_ICON_H

#include "Widget.h"
#include "Painter.h"
#include "Texture.h"

namespace r64fx{
    
class Icon{
    static Painter* p;
    
public:
    Texture2D* texture = nullptr;
    
    Size<float> size;
    
    Color color = { 1.0, 1.0, 1.0, 1.0 };
    
    Icon(Size<float> size = Icon::default_size, Texture2D* texture = nullptr)
    : texture(texture)
    , size(size)
    {}
    
    void render();
    
    static void init();
    
    static void cleanup();
    
    static Size<float> default_size;
};
    
}//namespace r64fx

#endif//R64FX_GUI_ICON_H