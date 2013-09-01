#include "MachineWidget.h"
#include "MachineScene.h"
#include "Texture.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;


namespace r64fx{
    

    
class FrontWidget : public Widget{
    Texture _surface_tex;
    
public:
    FrontWidget(Widget* parent = nullptr) : Widget(parent)
    {
        _surface_tex = Texture("textures/grainy_grey.png");
        if(!_surface_tex.isGood())
        {
            _surface_tex = Texture::defaultTexture();
        }
    }
    
    virtual ~FrontWidget() {}
    
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
    _front = new FrontWidget(nullptr);
    _back = new BackWidget(this);
    resize(100 * 19, 100.0 * 1.75);
    _front->resize(this->size());
    _back->resize(this->size());
}


MachineWidget::~MachineWidget()
{
    delete _front;
    delete _back;
}


void MachineWidget::render()
{
    Widget::render_children();
}


void MachineWidget::update()
{
#ifdef DEBUG
    assert(scene() != nullptr);
#endif//DEBUG
    
    removeWidget(0);
    
    if(scene()->to<MachineScene*>()->isShowingFrontSide())
    {
        appendWidget(_front);
    }
    else
    {
        appendWidget(_back);
    }
}

}//namespace r64fx 
