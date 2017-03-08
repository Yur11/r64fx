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
    fill_circle(dst, 0, 1, Color(0),   dstpos, size);
    fill_circle(dst, 0, 1, Color(255), dstpos + Point<int>(3, 3), size - 6);
    fill_bottom_triangle(dst, 0, 1, Color(255), dstpos, size);

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
    assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

    fill(dst, Color(31, 255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(dst, 1, 1, Color(0), dstpos + Point<int>(6, 6), size - 12);
}


// void draw_knob_handle(Image* dst, Point<int> dstpos, int size)
// {
// #ifdef R64FX_DEBUG
//     assert(dst != nullptr);
//     assert(dst->componentCount() == 2);
// #endif//R64FX_DEBUG
// 
//     Image c1(size, size, 1);
//     fill(&c1, Color(255), {dstpos.x(), dstpos.y(), size, size});
//     fill_circle(&c1, 0, 1, Color(0), dstpos + Point<int>(7, 7), size - 14);
// 
//     Image c2(size, size, 1);
//     fill(&c2, Color(255), {dstpos.x(), dstpos.y(), size, size});
//     fill_circle(&c2, 0, 1, Color(0), dstpos + Point<int>(10, 10), size - 20);
// 
//     Image c3(size, size, 1);
//     fill(&c3, Color(255), {dstpos.x(), dstpos.y(), size, size});
//     fill_circle(&c3, 0, 1, Color(0), dstpos + Point<int>(11, 11), size - 22);
// 
//     Image g1(size, size, 2);
//     fill_gradient_vert(&g1, 0, 1, 127, 31, {0, 7, size, size - 7});
//     copy(&g1, 1, 1, {0, 0}, &c1, 0, 1);
// 
//     Image g2(size, size, 2);
//     fill_gradient_vert(&g2, 0, 1, 223, 31, {0, 10, size, size - 10});
//     copy(&g2, 1, 1, {0, 0}, &c2, 0, 1);
// 
//     Image g3(size, size, 2);
//     fill_gradient_vert(&g3, 0, 1, 147, 107, {0, 11, size, size - 11});
//     copy(&g3, 1, 1, {0, 0}, &c3, 0, 1);
// 
//     fill(dst, Color(0, 255));
//     copy(dst, {0, 0}, &g1);
//     copy(dst, {0, 0}, &g2);
//     copy(dst, {0, 0}, &g3);
//     copy(dst, 1, 1, dstpos, &c1, 0, 1);
// }


void draw_knob_marker(Image* dst, Point<int> dstpos, int size)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

    fill(dst, Color(0, 255), ImgRect(dst));
    int hs = size / 2;
    fill(dst, Color(0, 127), {dstpos.x() + hs - 2, 10, 4, hs - 9});
    fill(dst, Color(255, 0), {dstpos.x() + hs - 1, 11, 2, hs - 11});
}


class KnobAnimGenerator{
    int m_size;
    Image c1, c2, c3, g1, g2, g3, marker;

public:
    KnobAnimGenerator(int size)
    : m_size(size)
    , c1(size, size, 1)
    , c2(size, size, 1)
    , c3(size, size, 1)
    , g1(size, size, 2)
    , g2(size, size, 2)
    , g3(size, size, 2)
    , marker(size, size, 2)
    {
        fill(&c1, Color(255));
        fill_circle(&c1, 0, 1, Color(0), Point<int>(7, 7), size - 14);

        fill(&c2, Color(255));
        fill_circle(&c2, 0, 1, Color(0), Point<int>(10, 10), size - 20);

        fill(&c3, Color(255));
        fill_circle(&c3, 0, 1, Color(0), Point<int>(11, 11), size - 22);

        fill(&marker, Color(0, 255));
        fill(&marker, Color(0, 127),  {size/2 - 2, 10, 4, size/2 - 10});
        fill(&marker, Color(255, 31), {size/2 - 1, 11, 2, size/2 - 12});
    }

    void genBackground(Image* dst, Point<int> dstpos, bool middle_notch)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG
        int hs = m_size / 2;

        fill(dst, Color(95, 255), {dstpos.x(), dstpos.y(), m_size, m_size});
        fill_circle(dst, 1, 1, Color(0),   dstpos, m_size);
        fill_circle(dst, 1, 1, Color(255), dstpos + Point<int>(3, 3), m_size - 6);
        fill_bottom_triangle(dst, 1, 1, Color(255), dstpos, m_size);

        if(middle_notch)
        {
            fill(dst, Color(0, 255), {dstpos.x() + hs - 2, dstpos.y(), 4, 5});
            fill(dst, Color(95, 0),  {dstpos.x() + hs - 1, dstpos.y(), 2, 5});
        }

//         {
//             Image img(m_size, m_size, 1);
//             fill(&img, 0, 1, 255);
//             fill(&img, 0, 1, 0, {hs - 1, 2, 3, 10});
// 
//             Transformation2D<float> t;
//             t.translate(+hs - 0.5f, +hs - 0.5f);
//             t.rotate(-M_PI * 0.75f);
//             t.translate(-hs + 0.5f, -hs + 0.5f);
//             copy(dst, 1, 1, t, &img, 0, 1);
//         }
    }

    void genKnob(Image* dst, Point<int> dstpos, float angle)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill_gradient_vert(&g1, 0, 1, 127, 31, {0, 7, m_size, m_size - 7});
        copy(&g1, {0, 0}, &c1, ChanShuf(1, 1, 0, 1));

        fill_gradient_vert(&g2, 0, 1, 223, 31, {0, 10, m_size, m_size - 10});
        copy(&g2, {0, 0}, &c2, ChanShuf(1, 1, 0, 1));

        fill_gradient_vert(&g3, 0, 1, 147, 107, {0, 11, m_size, m_size - 11});
        copy(&g3, {0, 0}, &c3, ChanShuf(1, 1, 0, 1));

        fill(dst, Color(0, 255));
        copy(dst, {0, 0}, &g1);
        copy(dst, {0, 0}, &g2);
        copy(dst, {0, 0}, &g3);
        copy(dst, dstpos, &c1, ChanShuf(1, 1, 0, 1));
    }

    void genMarker(Image* dst, Point<int> dstpos, float angle)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill(dst, Color(0, 255));

        int hs = m_size / 2;
        Transformation2D<float> t;
        t.translate(float(hs) - 0.5f, float(hs) - 0.5f);
        t.rotate(-(M_PI * 0.5f + angle));
        t.translate(float(-hs) + 0.5f, float(-hs) + 0.5f);
        copy(dst, t, &marker, ChanShuf(0, 2, 0, 2));
    }
};


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

    for(int i=0; i<8; i++)
    {
        int size = 40 + i * 2;
        int hs = size / 2;

        KnobAnimGenerator kanimg(size);

//         {
//             Image img(size, size, 2);
//             fill(&img, Color(95, 255));
//             fill(&img, 1, 1, 0, {size/2 - 2, 2, 3, 10});
// 
//             Image timg(size, size, 2);
//             fill(&timg, Color(95, 255));
//             Transformation2D<float> t;
//             t.translate(+hs - 0.5f, +hs - 0.5f);
//             t.rotate(+M_PI * 0.75f);
//             t.translate(-hs + 0.5f, -hs + 0.5f);
//             copy(&timg, 0, 2, t, &img, 0, 2);
//             p->putImage(&timg, {10 + (i * size + 20), 20});
//         }

        Image bg(size, size, 2);
        kanimg.genBackground(&bg, {0, 0}, false);
        p->putImage(&bg, {10 + (i * size + 20), 20});

        Image knob_base(size, size, 2);
        draw_knob_base(&knob_base, {0, 0}, size);
        p->putImage(&knob_base, {10 + (i * size + 20), 20});

        Image knob(size, size, 2);
        kanimg.genKnob(&knob, {0, 0}, 0.0f);
        p->putImage(&knob, {10 + (i * size + 20), 20});

        Image marker(size, size, 2);
        kanimg.genMarker(&marker, {0, 0}, m_angle);
        p->putImage(&marker, {10 + (i * size + 20), 20});
    }
}


void View_Project::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        m_angle += event->dy() * 0.025f;
        repaint();
    }

    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}

}//namespace r64fx
