#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Rect.hpp"
#include "WindowDefs.hpp"

namespace r64fx{

class Widget;

class Painter{

protected:
    virtual ~Painter() {}

public:
    static Painter* createNew(PainterType pt);

    static void destroy(Painter* wp);

    virtual PainterType mode() = 0;

    virtual void begin(Widget* widget) = 0;

    virtual void end() = 0;

    virtual void clear() = 0;

    virtual void setRect(Rect<int> rect) = 0;

    virtual void strokeRect(float r, float g, float b) = 0;

    virtual void fillRect(float r, float g, float b) = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP