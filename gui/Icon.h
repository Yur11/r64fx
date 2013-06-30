#ifndef R64FX_GUI_ICON_H
#define R64FX_GUI_ICON_H

#include "Widget.h"
#include "Texture.h"

namespace r64fx{
    
class Icon : public Widget{
    Texture* _texture = nullptr;
    
public:
    Icon(Texture* texture,  Widget* parent = nullptr) 
        : Widget(parent),  _texture(texture) {}
    
    Icon(std::string path, Widget* parent = nullptr) 
        : Widget(parent),  _texture(new Texture(path)) {}
        
    Icon(int width, int height, int channel_count, int mode, unsigned char* bytes, Widget* parent = nullptr)
        : Widget(parent), _texture(new Texture(width, height, channel_count, mode, bytes)) {}
    
    virtual void render();
    
    void resizeToFitTexture();

    static Icon* newDefaultIcon();
};
    
}//namespace r64fx

#endif//R64FX_GUI_ICON_H