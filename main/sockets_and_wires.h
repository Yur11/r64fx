#ifndef R64FX_SOCKETS_AND_WIRES_H
#define R64FX_SOCKETS_AND_WIRES_H

#include "Widget.h"

namespace r64fx{
    
class Wire;
    
/** @brief Base class for sockets that can be connected with wires. */
class Socket : public Widget{
    bool mouse_pressed = false;
    
public:
    Socket(Widget* parent = nullptr);
    
    float hole_radius = 0.25 * 100;
    float nut_radius = 0.43 * 100;
    
    Wire* wire = nullptr;
    
    virtual void render();
    
    virtual void update();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);

    Message drag_started;
};


/** @brief Connects two sockets */
class Wire : public Disposable{
    Socket* a = nullptr;
    Socket* b = nullptr;
    
public:
    Wire() { setVertexCount(32); }
    virtual ~Wire();
    
    Point<float> drag_position = {0.0, 0.0};

    std::vector<Point<float>> points;
    std::vector<Point<float>> vertices;
    
    void setVertexCount(int count);
    
    void update();
    
    inline void setA(Socket* s) { a = s; if(s) s->wire = this; }
    inline void setB(Socket* s) { b = s; if(s) s->wire = this; }
};
    
}//namespace r64fx

#endif//R64FX_SOCKETS_AND_WIRES_H