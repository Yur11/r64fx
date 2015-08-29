#include "Painter.hpp"
#include "Widget.hpp"


namespace r64fx{


class PainterCPU : public Painter{

public:
    virtual ~PainterCPU() {}

//     virtual PainterType mode() { return PainterType::Normal; }

    virtual void begin(Widget* widget) {}

    virtual void end() {}

    virtual void clear() {}

    virtual void setRect(Rect<int> rect) {}

    virtual void strokeRect(float r, float g, float b) {}

    virtual void fillRect(float r, float g, float b) {}
};


class PainterGL3 : public Painter{

public:
    virtual ~PainterGL3() {}

//     virtual PainterType mode() { return PainterType::GL3; }

    virtual void begin(Widget* widget) {}

    virtual void end() {}

    virtual void clear() {}

    virtual void setRect(Rect<int> rect) {}

    virtual void strokeRect(float r, float g, float b) {}

    virtual void fillRect(float r, float g, float b) {}
};


Painter* Painter::createNew()
{
//     switch(pt)
//     {
//         case PainterType::Normal:
            return new PainterCPU;
/*
        case PainterType::GL3:
            return new PainterGL3;

        default:
            return nullptr;
    }*/
}


void Painter::destroy(Painter* wp)
{
    delete wp;
}

}//namespace r64fx