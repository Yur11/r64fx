#include "RectVertices.h"
#include "Error.h"

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

#include <iostream>

using namespace std;

namespace r64fx{

RectVertices* RectVertices::_instance = nullptr;
    
void RectVertices::init()
{
#ifdef DEBUG
    assert(_instance == nullptr);
#endif//DEBUG
    
    _instance = new RectVertices();
}


void RectVertices::cleanup()
{
    delete[] _instance;
}


void RectVertices::setRect(Rect<float> rect, Rect<float> tex_rect)
{
    Painter::Vertex vertices[4] = {
        Painter::Vertex(rect.left,   rect.bottom,   tex_rect.left,   tex_rect.bottom),
        Painter::Vertex(rect.right,  rect.bottom,   tex_rect.right,  tex_rect.bottom),
        Painter::Vertex(rect.left,   rect.top,      tex_rect.left,   tex_rect.top),
        Painter::Vertex(rect.right,  rect.top,      tex_rect.right,  tex_rect.top)
    };
    
    bindBuffer();
    
    setVertices(vertices, 4);
    
    unbindBuffer();
}

}//namespace r64fx