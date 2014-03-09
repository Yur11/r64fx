#include "Socket.h"
#include "gui/Error.h"
#include "gui/MouseEvent.h"
#include "gui/Painter.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{

extern string data_prefix;
    
Texture Socket::texture;

void Socket::init()
{
    texture = Texture(data_prefix + "textures/socket.png");
#ifdef DEBUG
    assert(texture.isGood());
#endif//DEBUG
}
    
    
Socket::Socket(Widget* parent) : Widget(parent), pv(4)
{
}


void Socket::render()
{    
    glEnable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_FOR_GL_ERRORS;
    
//     Painter::enable();
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    Painter::setTexture(Socket::texture.id());
    Painter::useCurrent2dProjection();
    
    pv.bindArray();
    pv.render(GL_TRIANGLE_STRIP);
    pv.unbindArray();
    
//     Painter::disable();

    glDisable(GL_BLEND);
    CHECK_FOR_GL_ERRORS;
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
    
    pv.bindBuffer();
    pv.setData(data);
    pv.unbindBuffer();
}


void Socket::mousePressEvent(MouseEvent* event)
{
    clicked(this);
}


void Socket::mouseReleaseEvent(MouseEvent* event)
{
}


void Socket::mouseMoveEvent(MouseEvent* event)
{

}
    
}//namespace r64fx
