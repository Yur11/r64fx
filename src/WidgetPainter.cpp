#include "WidgetPainter.hpp"
#include "Widget.hpp"


namespace r64fx{


class WidgetPainterCPU : public WidgetPainter{

public:
    virtual ~WidgetPainterCPU() {}

    virtual PainterType mode() { return PainterType::Normal; }

    virtual void begin(Widget* widget) {}

    virtual void end() {}

    virtual void clear() {}

    virtual void setRect(Rect<int> rect) {}

    virtual void strokeRect(float r, float g, float b) {}

    virtual void fillRect(float r, float g, float b) {}
};


class WidgetPainterGL3 : public WidgetPainter{

public:
    virtual ~WidgetPainterGL3() {}

    virtual PainterType mode() { return PainterType::GL3; }

    virtual void begin(Widget* widget) {}

    virtual void end() {}

    virtual void clear() {}

    virtual void setRect(Rect<int> rect) {}

    virtual void strokeRect(float r, float g, float b) {}

    virtual void fillRect(float r, float g, float b) {}
};


WidgetPainter* WidgetPainter::createNew(PainterType pt)
{
    switch(pt)
    {
        case PainterType::Normal:
            return new WidgetPainterCPU;

        case PainterType::GL3:
            return new WidgetPainterGL3;

        default:
            return nullptr;
    }
}


void WidgetPainter::destroy(WidgetPainter* wp)
{
    delete wp;
}

}//namespace r64fx