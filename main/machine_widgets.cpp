#include "machine_widgets.h"
#include "machine_scenes.h"


#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;


namespace r64fx{

class BackWidget : public Widget{
    Texture _surface_tex;
    
public:
    BackWidget(Widget* parent = nullptr) : Widget(parent)
    {
        _surface_tex = Texture::defaultTexture();
    }
    
    virtual ~BackWidget() {}
    
    virtual void render()
    {
        glEnable(GL_TEXTURE_2D);
        _surface_tex.bind();
        glBegin(GL_POLYGON);
            glTexCoord2f(0.0, 0.0);
            glVertex2f(0.0, 0.0);
            
            glTexCoord2f(0.0, height() / _surface_tex.height());
            glVertex2f(0.0, height());
            
            glTexCoord2f(width() / _surface_tex.width(), height() / _surface_tex.height());
            glVertex2f(width(), height());
            
            glTexCoord2f(width() / _surface_tex.width(), 0.0);
            glVertex2f(width(), 0.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
};
    
    
MachineWidget::MachineWidget(Widget* parent) : Widget(parent)
{
    resize(19, 1.75);
    _surface_tex = Texture("textures/grainy_grey.png");
    if(!_surface_tex.isGood())
    {
        _surface_tex = Texture::defaultTexture();
    }
}


MachineWidget::~MachineWidget()
{
}


void MachineWidget::render()
{
    glEnable(GL_TEXTURE_2D);
    _surface_tex.bind();
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);
        
        glTexCoord2f(0.0, height() / _surface_tex.height());
        glVertex2f(0.0, height());
        
        glTexCoord2f(width() / _surface_tex.width(), height() / _surface_tex.height());
        glVertex2f(width(), height());
        
        glTexCoord2f(width() / _surface_tex.width(), 0.0);
        glVertex2f(width(), 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

}//namespace r64fx 
