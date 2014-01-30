#include "Socket.h"
#include "gui/MouseEvent.h"
#include "gui/TexturedRect.h"

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
    
    TexturedRect::render(
        0.0, 0.0, width(), height(),
        0.0, 0.0, 1.0, 1.0,
        Socket::texture.id()
    );
    
    glUseProgram(0);
    
    glDisable(GL_BLEND);    
}


void Socket::update()
{
    setWidth(radius * 2.0);
    setHeight(radius * 2.0);
}


void Socket::mousePressEvent(MouseEvent* event)
{

}


void Socket::mouseMoveEvent(MouseEvent* event)
{

}
    
}//namespace r64fx
