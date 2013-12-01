#ifndef R64FX_SOCKETS_AND_WIRES_H
#define R64FX_SOCKETS_AND_WIRES_H

#include "gui/Widget.h"

namespace r64fx{
    
class Wire;
    
/** @brief Base class for sockets that can be connected with wires. */
class Socket : public Widget{
    bool mouse_pressed = false;
    
public:
    Socket(Widget* parent = nullptr);
    
    float hole_radius = 0.25 * 100;
    float nut_radius = 0.30 * 100;
    
    /** This affects only the rendering. */
    bool is_plugged = false;
    
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
    static GLuint _texture;
    std::vector<Point<float>> points;
    
public:
    Wire() { setVertexCount(16); }
    virtual ~Wire();
    
    Point<float> drag_position = {0.0, 0.0};
    
    float width = 10.0;
    
    Color color = Color(1.0, 1.0, 1.0);

    struct Vertex{
        Point<float> position;
        float elevation;
        Color color;
        float perpendicular_angle = 0.0;
        Point<float> perp_a, perp_b;
        
        Vertex(Point<float> position = {0, 0}, float elevation = 0.0, Color color = Color())
        : position(position)
        , elevation(elevation)
        , color(color)
        {}
    };
    
    std::vector<Vertex> vertices;
    
    void setVertexCount(int count);
    
    void render();
    
    void update();
    
    inline void setA(Socket* s) { a = s; if(s) s->wire = this; }
    inline void setB(Socket* s) { b = s; if(s) s->wire = this; }
    
    static void init();
};
    
}//namespace r64fx

#endif//R64FX_SOCKETS_AND_WIRES_H