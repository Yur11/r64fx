#ifndef R64FX_MAIN_SOCKET_WIRES_H
#define R64FX_MAIN_SOCKET_WIRES_H

#include "gui/Widget.h"
#include "gui/Texture.h"
#include "gui/Shader.h"

namespace r64fx{
    
class Wire;
    
/** @brief Base class for sockets that can be connected with wires. */
class Socket : public Widget{
    friend class Wire;
    bool mouse_pressed = false;
    static Texture texture;

    Wire* _wire = nullptr;
    
public:
    static void init();
    
    Socket(Widget* parent = nullptr);
    
    float radius = 0.30 * 100;

    inline Wire* wire() const { return _wire; }
    
    inline bool isPlugged() const { return _wire != nullptr; }
    
    virtual void render(RenderingContextId_t context_id);
    
    virtual void update();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);

    inline Point<float> center() const 
    { 
        return {
            float(width() * 0.5),
            float(height() * 0.5)
        };
    }
    
    Message drag_started;
};

    
}//namespace r64fx

#endif//R64FX_SOCKETS_AND_WIRES_H