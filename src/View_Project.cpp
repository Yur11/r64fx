#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

void draw_solid_horse_shoe(Image* dst, Point<int> dstpos, int size, bool middle_notch = true)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 1);
#endif//R64FX_DEBUG
    int hs = size / 2;

    Point<float> center(dstpos.x() + hs - 0.5f, dstpos.y() + hs - 0.5f);
    fill(dst, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(dst, Color(0),   center, hs - 1 + 0.5f);
    fill_circle(dst, Color(255), center, hs - 4 + 0.5f);
    fill_bottom_triangle(dst, Color(255), dstpos, size);

    if(middle_notch)
    {
        fill(dst, Color(255), {dstpos.x() + hs - 2, dstpos.y(), 4, 5});
        fill(dst, Color(0), {dstpos.x() + hs - 1, dstpos.y(), 2, 5});
    }
}


void draw_knob_base(Image* dst, Point<int> dstpos, int size)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 4);
#endif//R64FX_DEBUG
    int hs = size / 2;

    Point<float> center(dstpos.x() + hs - 0.5f, dstpos.y() + hs - 0.5f);
    fill(dst, Color(255, 255, 255, 255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(dst, Color(63, 63, 63, 63), center, hs - 6 + 0.5f);
    fill_circle(dst, Color(0, 0, 0, 0), center, hs - 7 + 0.5f);
}


void draw_knob_handle(Image* dst, Point<int> dstpos, int size)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 4);
#endif//R64FX_DEBUG
    int hs = size / 2;

    Point<float> center(dstpos.x() + hs - 0.5f, dstpos.y() + hs);

    Image c1(size, size, 1);
    fill(&c1, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(&c1, Color(0), center, hs - 8 + 0.5f);

    Image c2(size, size, 1);
    fill(&c2, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(&c2, Color(0), center, hs - 10 + 0.5f);

    Image c3(size, size, 1);
    fill(&c3, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(&c3, Color(0), center, hs - 11 + 0.5f);

    Image g1(size, size, 4);
    fill_gradient_vert(&g1, 127, 37, 0, 3);
    copy_component(&g1, 3, {0, 0}, &c1, 0);

    Image g2(size, size, 4);
    fill_gradient_vert(&g2, 197, 77, 0, 3);
    copy_component(&g2, 3, {0, 0}, &c2, 0);

    Image g3(size, size, 4);
    fill_gradient_vert(&g3, 147, 107, 0, 3);
    copy_component(&g3, 3, {0, 0}, &c3, 0);

    fill(dst, Color(255, 255, 255, 255));
    copy_rgba(dst, {0, 0}, &g1);
    copy_rgba(dst, {0, 0}, &g2);
    copy_rgba(dst, {0, 0}, &g3);
    copy_component(dst, 3, dstpos, &c1, 0);
}


View_Project::View_Project(Widget* parent) : Widget(parent)
{
    
}


View_Project::~View_Project()
{
    
}


void View_Project::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    for(int i=0; i<16; i++)
    {
        int size = 28 + i * 2;

        Image horse_shoe(size, size, 1);
        draw_solid_horse_shoe(&horse_shoe, {0, 0}, size);

        Image img(size, size, 4);
        fill(&img, Color(95, 95, 95, 0));
        copy_component(&img, 3, {0, 0}, &horse_shoe, 0);

//         p->fillRect({20 + (i * size + 20), 20, size, size}, Color(127, 0, 0, 0));
        p->putImage(&img, {10 + (i * size + 20), 20});

        Image knob_base(size, size, 4);
        draw_knob_base(&knob_base, {0, 0}, size);
        p->putImage(&knob_base, {10 + (i * size + 20), 20});

        Image knob_handle(size, size, 4);
        draw_knob_handle(&knob_handle, {0, 0}, size);
        p->putImage(&knob_handle, {10 + (i * size + 20), 20});
    }
}


void View_Project::mouseMoveEvent(MouseMoveEvent* event)
{
    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}

}//namespace r64fx
