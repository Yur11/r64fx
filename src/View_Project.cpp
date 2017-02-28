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

    fill(dst, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(dst, Color(0),   dstpos, size, 0, 1);
    fill_circle(dst, Color(255), dstpos + Point<int>(3, 3), size - 6, 0, 1);
    fill_bottom_triangle(dst, Color(255), dstpos, size);

    int hs = size / 2;
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

    fill(dst, Color(31, 31, 31, 255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(dst, Color(0), dstpos + Point<int>(6, 6), size - 12, 3, 1);
}


void draw_knob_handle(Image* dst, Point<int> dstpos, int size)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 4);
#endif//R64FX_DEBUG

    Image c1(size, size, 1);
    fill(&c1, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(&c1, Color(0), dstpos + Point<int>(7, 7), size - 14, 0, 1);

    Image c2(size, size, 1);
    fill(&c2, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(&c2, Color(0), dstpos + Point<int>(10, 10), size - 20, 0, 1);

    Image c3(size, size, 1);
    fill(&c3, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(&c3, Color(0), dstpos + Point<int>(11, 11), size - 22, 0, 1);

    Image g1(size, size, 4);
    fill_gradient_vert(&g1, 127, 31, 0, 3, {0, 7, size, size - 7});
    copy_component(&g1, 3, 1, {0, 0}, &c1, 0);

    Image g2(size, size, 4);
    fill_gradient_vert(&g2, 223, 31, 0, 3, {0, 10, size, size - 10});
    copy_component(&g2, 3, 1, {0, 0}, &c2, 0);

    Image g3(size, size, 4);
    fill_gradient_vert(&g3, 147, 107, 0, 3, {0, 11, size, size - 11});
    copy_component(&g3, 3, 1, {0, 0}, &c3, 0);

    fill(dst, Color(0, 0, 0, 255));
    copy_rgba(dst, {0, 0}, &g1);
    copy_rgba(dst, {0, 0}, &g2);
    copy_rgba(dst, {0, 0}, &g3);
    copy_component(dst, 3, 1, dstpos, &c1, 0);
}


void draw_knob_marker(Image* dst, Point<int> dstpos, int size)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 4);
#endif//R64FX_DEBUG

    fill(dst, Color(0, 0, 0, 255));
    int hs = size / 2;
    fill(dst, Color(0, 0, 0, 127), {dstpos.x() + hs - 2, 10, 4, hs - 9});
    fill(dst, Color(255, 255, 255, 0), {dstpos.x() + hs - 1, 11, 2, hs - 11});
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

    for(int i=0; i<14; i++)
    {
        int size = 38 + i * 2;

        Image horse_shoe(size, size, 1);
        draw_solid_horse_shoe(&horse_shoe, {0, 0}, size);

        Image img(size, size, 4);
        fill(&img, Color(95, 95, 95, 0));
        copy_component(&img, 3, 1, {0, 0}, &horse_shoe, 0);

        p->putImage(&img, {10 + (i * size + 20), 20});

        Image knob_base(size, size, 4);
        draw_knob_base(&knob_base, {0, 0}, size);
        p->putImage(&knob_base, {10 + (i * size + 20), 20});

        Image knob_handle(size, size, 4);
        draw_knob_handle(&knob_handle, {0, 0}, size);
        p->putImage(&knob_handle, {10 + (i * size + 20), 20});

        Image marker(size, size, 4);
        draw_knob_marker(&marker, {0, 0}, size);
        p->putImage(&marker, {10 + (i * size + 20), 20});
    }

    Image img1(44, 44, 1);
    fill(&img1, Color(0));
    fill(&img1, Color(127), {11, 11, 22, 12});
    fill(&img1, Color(255), {11, 22, 22, 12});
    p->putImage(&img1, {20, 100});

    Image img3(44, 44, 3);
    fill(&img3, Color(255,  0, 0),   {0,   0, 22, 22});
    fill(&img3, Color(0,  255, 0),   {22,  0, 22, 22});
    fill(&img3, Color(255,  0, 255), {0,  22, 22, 22});
    fill(&img3, Color(0,    0, 0),   {22, 22, 22, 22});
    p->putImage(&img3, {53, 144});

    Image img2(44, 44, 2);
    fill(&img2, Color(0, 127));
    p->putImage(&img2, {42, 110});

    Image img4(44, 44, 4);
    fill(&img4, Color(255, 255, 255, 255));
    fill_circle(&img4, Color(0), {0, 0}, 44, 3, 1);
    p->putImage(&img4, {20, 170});
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
