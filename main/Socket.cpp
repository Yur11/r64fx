#include "Socket.h"
#include "gui/MouseEvent.h"
#include "gui/Painter.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{

extern string data_prefix;
    
Texture2D* Socket::texture = nullptr;

void Socket::init()
{
//     texture = Texture("./data/textures/socket.png");
// #ifdef DEBUG
//     assert(texture.isGood());
// #endif//DEBUG
}
    
    
Socket::Socket(Widget* parent) : Widget(parent), p(4)
{
}


void Socket::render()
{    
    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    Painter::setTexture(Socket::texture->glName());
//     Painter::useCurrent2dProjection();
    
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();
    
//     Painter::disable();

    gl::Disable(GL_BLEND);
}


void Socket::update()
{
    setWidth(radius * 2.0);
    setHeight(radius * 2.0);
    
    static float data[16] = {
        0.0, 0.0,
        width(), 0.0,
        0.0, height(),
        width(), height(),
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p.bindBuffer();
    p.setData(data);
    p.unbindBuffer();
}


void Socket::mousePressEvent(MouseEvent* event)
{
//     clicked(this);
}


void Socket::mouseReleaseEvent(MouseEvent* event)
{
}


void Socket::mouseMoveEvent(MouseEvent* event)
{

}
    
}//namespace r64fx
