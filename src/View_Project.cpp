#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "StringUtils.hpp"
#include "TextPainter.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

#include "Atlas.hpp"
#include "Widget_Knob.hpp"

namespace r64fx{

Font* g_font = nullptr;


class KnobAnimGenerator{
    int m_size;
    Image marker;

    inline int halfSize() const { return m_size >> 1; }

public:
    KnobAnimGenerator(int size)
    : m_size(size)
    , marker(size, size, 2)
    {
        fill(&marker, Color(0, 255));
        fill({&marker, {size/2 - 2, 7, 4, size/2 - 7}}, Color(0, 127));
        fill({&marker, {size/2 - 1, 8, 2, size/2 - 9}}, Color(255, 31));
        for(int i=0; i<4; i++)
        {
            fill({&marker, {size/2 - 1, 7 + i, 2, 1}}, 1, 1, 191 - 24 * i);
        }
    }

    void genKnob(Image* dst, Point<int> dstpos)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill({dst, {dstpos.x(), dstpos.y(), m_size, m_size}}, Color(31, 255));

        Image alpha_mask(m_size, m_size, 1);
        fill(&alpha_mask, 0, 1, 255);

        genDecorationSolidHorseShoe(&alpha_mask);
//         genDecorationSegmentedRing(&alpha_mask, 8);

        genKnobCenter(dst, &alpha_mask, dstpos);
        copy(dst, dstpos, &alpha_mask, ChanShuf(1, 1, 0, 1));
    }

private:
    void genKnobCenter(Image* dst, Image* alpha_mask, Point<int> dstpos)
    {
        {
            Image c0(m_size, m_size, 1);
            fill(&c0, Color(255));
            fill_circle(&c0, 0, 1, Color(0), {6, 6}, m_size - 12);

            copy(alpha_mask, {0, 0}, &c0, PixOpMin());

            Image layer(m_size, m_size, 2);
            fill(&layer, Color(0, 255));
            copy(&layer, {0, 0}, &c0, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }

        {
            Image c1(m_size, m_size, 1);
            fill(&c1, Color(255));
            fill_circle(&c1, 0, 1, Color(0), {7, 7}, m_size - 14);

            Image layer(m_size, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 127, 31, {0, 7, m_size, m_size - 7});
            copy(&layer, {0, 0}, &c1, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }

        {
            Image c2(m_size, m_size, 1);
            fill(&c2, Color(255));
            fill_circle(&c2, 0, 1, Color(0), {10, 10}, m_size - 20);

            Image layer(m_size, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 223, 31, {0, 10, m_size, m_size - 10});
            copy(&layer, {0, 0}, &c2, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }

        {
            Image c3(m_size, m_size, 1);
            fill(&c3, Color(255));
            fill_circle(&c3, 0, 1, Color(0), {11, 11}, m_size - 22);

            Image layer(m_size, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 147, 107, {0, 11, m_size, m_size - 11});
            copy(&layer, {0, 0}, &c3, ChanShuf(1, 1, 0, 1));

            copy(dst, dstpos, &layer);
        }
    }

    void genBaseRing(Image* ring)
    {
        fill(
            ring, 0, 1, 255
        );
        fill_circle(
            ring, 0, 1, Color(0), {0, 0}, m_size
        );
        fill_circle(
            ring, 0, 1, Color(255), {3, 3}, m_size - 6
        );
    }

    void genBaseHorseShoe(Image* horse_shoe)
    {
        genBaseRing(horse_shoe);
        fill_bottom_triangle(
            horse_shoe, 0, 1, Color(255), {0, 0}, m_size
        );
    }

    void genDecorationSolidHorseShoe(Image* alpha_mask)
    {
        Image horse_shoe(m_size, m_size, 1);
        genBaseHorseShoe(&horse_shoe);

        Image tick(m_size, m_size, 1);
        fill(&tick, 0, 1, 255);
        fill({&tick, {m_size/2 - 1, 2, 3, 10}}, 0, 1, 0);
        rotateAndCopy(&horse_shoe, &tick, -M_PI * 0.75f, PixOpMin());

        mirror_left2right(&horse_shoe);
        copy(alpha_mask, {0, 0}, &horse_shoe, PixOpMin());

        fill({alpha_mask, {halfSize() - 2, 0, 4, 6}}, 0, 1, 255);
        fill({alpha_mask, {halfSize() - 1, 0, 2, 5}}, 0, 1, 0);
    }

    void genDecorationSegmentedRing(Image* alpha_mask, int nsegments)
    {
        Image ring(m_size, m_size, 1);
        genBaseRing(&ring);

        Image tick(m_size, m_size, 1);
        fill(&tick, 0, 1, 0);
        fill({&tick, {m_size/2 - 1, 0, 2, 6}}, 0, 1, 255);
        float angle_range = M_PI * 2.0f;
        float angle_step = angle_range / float(nsegments);
        float angle = -M_PI;
        for(int i=0; i<nsegments; i++)
        {
            rotateAndCopy(&ring, &tick, angle, PixOpAdd());
            angle += angle_step;
        }

        copy(alpha_mask, {0, 0}, &ring, PixOpMin());
    }

    void rotateAndCopy(Image* dst, Image* src, float angle, PixelOperation pixop)
    {
        Transformation2D<float> t;
        t.translate(+halfSize() - 0.5f, +halfSize() - 0.5f);
        t.rotate(angle);
        t.translate(-halfSize() + 0.5f, -halfSize() + 0.5f);
        copy(dst, t, src, pixop);
    }

public:
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
        copy(dst, t, &marker);
    }

    void genIndicationFrame(Image* dst, Point<int> dstpos, float min_angle, float max_angle)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG
        fill(dst, Color(255, 255));
    }
};


View_Project::View_Project(Widget* parent) : Widget(parent)
{
    if(!g_font)
    {
        g_font = new Font("mono", 14);
    }

    for(int i=0; i<8; i++)
    {
        auto knob = new Widget_Knob(KnobStyle::Unipolar, 44 + i*4, this);
        knob->setPosition({50 + i*70, 350});
    }
}


View_Project::~View_Project()
{
    if(g_font)
    {
        delete g_font;
        g_font = nullptr;
    }
}


float normalize_angle(float angle)
{
    while(angle >= (M_PI * 2.0f))
        angle -= (M_PI * 2.0f);
    while(angle < 0)
        angle += (M_PI * 2.0f);
    return angle;
}


void View_Project::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));
//     Image img(256, 256, 3);
//     fill(&img, Color(0, 0, 0));
// 
//     srand(time(nullptr));
// 
//     vector<Rect<short>> rects;
//     Atlas atlas(0, 0, img.width(), img.height());
//     for(int i=0; i<20; i++)
//     {
//         short w = (rand() % 7 + 1) * 20;
//         short h = (rand() % 5 + 1) * 10;
//         short r = (rand() % 16) << 4;
//         short g = (rand() % 16) << 4;
//         short b = (rand() % 16) << 4;
// 
//         auto rect = atlas.allocRect({w, h});
//         assert(rect.width() >= 0);
//         if(rect.width() == 0)
//         {
//             assert(rect.height() == 0);
//             cout << "No room for: " << i << " -> " << w << "x" << h << "\n";
//             continue;
//         }
//         assert(rect.height() > 0);
// 
//         rects.push_back(rect);
//         fill({&img, Rect<int>(rect.x(), rect.y(), rect.width(), rect.height())}, Color(r, g, b));
//     }
// 
//     p->putImage(&img, {50, 100});
/*
    for(auto &rect : rects)
    {
        atlas.freeRect(rect);
    }*/

    constexpr float rcp = 1.0f / 32.0f;

    int size = 64;
    KnobAnimGenerator kanimg(size);

    Image teximg(size*8, size*4, 2);
    Image subimg(size, size, 2);
    fill(&teximg, Color(0, 255));
    for(int y=0; y<4; y++)
    {
        for(int x=0; x<8; x++)
        {
            fill(&subimg, Color(0, 255));
            kanimg.genMarker(&subimg, {0, 0}, (x + y*8) * rcp * M_PI * 0.5f - M_PI * 0.5f);
            copy(&teximg, {x*size, y*size}, &subimg, PixOpReplace());
        }
    }
    p->putImage(&teximg, {10, 10});

    childrenPaintEvent(event);
}


void View_Project::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        mi -= event->dy();
        if(mi < 0)
            mi = 0;
        repaint();
    }

    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}


}//namespace r64fx
