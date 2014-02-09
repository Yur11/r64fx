#include "Socket.h"
#include "gui/MouseEvent.h"
#include "gui/RectPainter.h"

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
    
    
Socket::Socket(Widget* parent) : Widget(parent)
{
}


void Socket::render()
{    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    RectPainter::prepare();
    RectPainter::setTexCoords(0.0, 0.0, 1.0, 1.0);
    RectPainter::setTexture(Socket::texture.id());
    RectPainter::setColor(1.0, 1.0, 1.0, 1.0);
    RectPainter::setCoords(0.0, 0.0, width(), height());
    RectPainter::render();

    glDisable(GL_BLEND);    
}


void Socket::update()
{
    setWidth(radius * 2.0);
    setHeight(radius * 2.0);
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
