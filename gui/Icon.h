#ifndef R64FX_GUI_ICON_H
#define R64FX_GUI_ICON_H

#include "Widget.h"
#include "Texture.h"

namespace r64fx{
    
class Icon : public Widget{
    Texture* _texture = nullptr;
    
public:
    Icon(Texture* texture,  Widget* parent = nullptr) : Widget(parent),  _texture(texture) {}
    
    virtual void render();
    
    void resizeToFitTexture();
};
    
}//namespace r64fx

#endif//R64FX_GUI_ICON_H