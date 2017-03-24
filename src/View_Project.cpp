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
        copy(dst, t, &marker, ChanShuf(0, 2, 0, 2));
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
    constexpr float ang_coeff = (2.0f * M_PI) / 256.0f;

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    for(int i=0; i<13; i++)
    {
        int size = 40 + i * 2;

        KnobAnimGenerator kanimg(size);

        Image knob(size, size, 2);
        kanimg.genKnob(&knob, {0, 0});
        p->putImage(&knob, {10 + (i * size + 20), 20});

        Image marker(size, size, 2);
        kanimg.genMarker(&marker, {0, 0}, mi * -ang_coeff + M_PI * 0.5f);
        p->putImage(&marker, {10 + (i * size + 20), 20});
    }

//     int size = 64;
//     int hs = size/2;
// 
//     Image circleimg(size, size, 1);
//     fill(&circleimg, Color(0));
//     fill_circle(&circleimg, 0, 1, Color(255), {0, 0}, size);
//     fill_circle(&circleimg, 0, 1, Color(0), {hs - 4, hs - 4}, 8);
// 
//     auto textimg = text2image(num2str(mi), TextWrap::None, g_font);
//     p->blendColors({200, 100}, Colors(Color(0)), textimg);
//     delete textimg;
// 
//     double pircp = 0.5 / M_PI;
//     Image atanimg(hs, hs, 1);
//     for(int y=0; y<hs; y++)
//     {
//         for(int x=0; x<hs; x++)
//         {
//             auto ang = 0.0;
//             if(x || y)
//                 ang = atan2(y, x) * pircp * 255.0;
//             atanimg(x, y)[0] = (unsigned char) ang;
//         }
//     }
//     flip_hori(&atanimg);
//     flip_vert(&atanimg);
//     p->putImage(&atanimg, {300, 100});
// 
//     Image thrimg(hs, hs, 1);
//     for(int y=0; y<hs; y++)
//     {
//         for(int x=0; x<hs; x++)
//         {
//             if(atanimg(x, y)[0] < mi)
//                 thrimg(x, y)[0] = 255;
//             else
//                 thrimg(x, y)[0] = 0;
//         }
//     }
//     p->putImage(&thrimg, {400, 100});
// 
// 
//     Image tick(hs, hs, 1);
//     fill(&tick, Color(0));
//     fill({&tick, {0, 0, 1, hs}}, Color(255));
// 
//     int offset = 63;
//     Image resimg(size, size, 1);
//     for(int y=0; y<size; y++)
//     {
//         for(int x=0; x<size; x++)
//         {
//             int sx = size - x - 1;
//             int sy = size - y - 1;
//             int hx = (x >= hs ? 1 : 0);
//             int hy = (y >= hs ? 1 : 0);
//             int tx = (hx ? sx : x);
//             int ty = (hy ? sy : y);
//             int xx = (hx ^ hy ? ty : tx);
//             int yy = (hx ^ hy ? tx : ty);
//             hy = hy | (hy << 1);
//             int val = (((hx ^ hy) & 3) << 6) | atanimg(xx, yy)[0];
//             resimg(x, y)[0] = (((val + offset) & 255) < mi ? 255 : 0);
//         }
//     }
// 
// 
//     Image tickimg(size, size, 1);
//     fill(&tickimg, Color(0));
//     fill({&tickimg, {0, hs-1, hs, 2}},Color(255));
// 
//     Image trimg(size, size, 1);
//     {
//         fill(&trimg, Color(0));
//         Transformation2D<float> t;
//         t.translate(+hs - 0.5f, +hs - 0.5f);
//         t.rotate(((mi + 255 - offset) & 255) * ang_coeff);
//         t.translate(-hs + 0.5f, -hs + 0.5f);
//         copy(&trimg, t, &tickimg, PixOpReplace());
//     }
// 
//     Image trtex(size * 8, size, 2);
//     {
//         fill(&trtex, Color(0, 255));
//         for(int i=0; i<256; i++)
//         {
//             Transformation2D<float> t;
//             t.translate(+hs - 0.5f, +hs - 0.5f);
//             t.rotate(((i + 255 - offset) & 255) * ang_coeff);
//             t.translate(-hs + 0.5f, -hs + 0.5f);
// 
//             Image trtick(size, size, 1);
//             fill(&trtick, Color(0));
//             copy(&trtick, t, &tickimg, PixOpReplace());
// 
//             invert(&trtick, &trtick);
//             copy(&trtex, {size * (i & 7), 0}, &trtick, PixOpMin() | ChanShuf(1, 1, 0, 1));
//             threshold(&trtick, &trtick, Color(i), Color(0), 254);
//             copy(&trtex, {size * (i & 7), 0}, &trtick, PixOpMax() | ChanShuf(0, 1, 0, 1));
//         }
//     }
// 
//     Image trteximg(size, size, 1);
//     {
//         fill(&trteximg, Color(0));
//         for(int y=0; y<size; y++)
//         {
//             for(int x=0; x<size; x++)
//             {
//                 auto texel = trtex(x + (size * (mi & 7)), y);
//                 auto dstpx = trteximg(x, y);
// 
//                 if(texel[0] == mi)
//                 {
//                     float alpha = float(texel[1]) / 255.0f;
//                     float one_minus_alpha = 1.0f - alpha;
//                     float val = float(dstpx[0]) * alpha + 255.0f * one_minus_alpha;
//                     trteximg(x, y)[0] = (unsigned char) val;
//                 }
//             }
//         }
//     }
// 
//     copy(&resimg, {0, 0}, &trimg, PixOpAdd());
//     copy(&resimg, {0, 0}, &circleimg, PixOpMin());
// 
//     p->putImage(&resimg, {500, 100});
//     p->putImage(&trimg, {500, 200});
//     p->putImage(&trtex, {500, 300});
//     p->putImage(&trteximg, {400, 300});

    Image img(256, 256, 3);
    fill(&img, Color(0, 0, 0));

    srand(time(nullptr));

    vector<Rect<short>> rects;
    Atlas atlas(0, 0, img.width(), img.height());
    for(int i=0; i<20; i++)
    {
        short w = (rand() % 7 + 1) * 20;
        short h = (rand() % 5 + 1) * 10;
        short r = (rand() % 16) << 4;
        short g = (rand() % 16) << 4;
        short b = (rand() % 16) << 4;

        auto rect = atlas.allocRect({w, h});
        assert(rect.width() >= 0);
        if(rect.width() == 0)
        {
            assert(rect.height() == 0);
            cout << "No room for: " << i << " -> " << w << "x" << h << "\n";
            continue;
        }
        assert(rect.height() > 0);

        rects.push_back(rect);
        fill({&img, Rect<int>(rect.x(), rect.y(), rect.width(), rect.height())}, Color(r, g, b));
    }

    p->putImage(&img, {50, 100});

    for(auto &rect : rects)
    {
        atlas.freeRect(rect);
    }
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
