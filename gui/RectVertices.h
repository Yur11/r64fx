#ifndef R64FX_GUI_RECT_VERTICES_H
#define R64FX_GUI_RECT_VERTICES_H

#include "geometry.h"
#include "Painter.h"
#include "Vertices.h"

namespace r64fx{
    
/** @brief Shared Vertices instance for drawing rectangles with the Painter. */
class RectVertices : public Vertices<Painter::Vertex>{
    static RectVertices* _instance;
    
public:
    static void init();
  
    static void cleanup();
    
    RectVertices(GLenum usage = GL_STATIC_DRAW)
    : Vertices<Painter::Vertex>(4, nullptr, usage)
    {}
    
    inline static RectVertices* instance() { return _instance; }
    
    void setRect(Rect<float> rect, Rect<float> tex_rect = { 0.0, 1.0, 1.0, 0.0 });
};



    
}//namespace r64fx

#endif//R64FX_GUI_RECT_VERTICES_H