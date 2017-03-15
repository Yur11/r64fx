#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

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

//         genDecorationSolidHorseShoe(&alpha_mask);
        genDecorationSegmentedRing(&alpha_mask, 8);

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
            horse_shoe, 1, 1, Color(255), {0, 0}, m_size
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

}


View_Project::~View_Project()
{

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

//     for(int i=0; i<13; i++)
//     {
//         int size = 40 + i * 2;
// 
//         KnobAnimGenerator kanimg(size);
// 
//         Image knob(size, size, 2);
//         kanimg.genKnob(&knob, {0, 0});
//         p->putImage(&knob, {10 + (i * size + 20), 20});
// 
//         Image marker(size, size, 2);
//         kanimg.genMarker(&marker, {0, 0}, m_angle);
//         p->putImage(&marker, {10 + (i * size + 20), 20});
//     }

    int size = 64;
    int hs = size/2;

    Image maskimg(size*2, size, 4);
    fill({&maskimg, {0, 0, size, size}}, Color(0, 0, 0, 0));
    fill({&maskimg, {size, 0, size, size}}, Color(255, 255, 255, 255));

    int nticks = 32;
    float angle_range = M_PI * 2.0f;
    float angle_step = angle_range / float(nticks);
    float angle_shift_step = angle_range / float(nticks * 4);
    float angle_shift = 0.0f;
    for(int n=0; n<4; n++)
    {
        Image tick(size, size, 1);
        fill(&tick, Color(0));

        Image color(size, size, 1);
        Image alpha(size, size, 1);

        float angle = 0.0f;
        for(int i=0; i<nticks; i++)
        {
            fill({&tick, {hs - 1, 0, 2, 5}}, Color(255));
            fill(&alpha, Color(0));

            Transformation2D<float> t;
            t.translate(float(hs) - 0.5f, float(hs) - 0.5f);
            t.rotate(angle + angle_shift);
            t.translate(float(-hs) + 0.5f, float(-hs) + 0.5f);
            copy(&alpha, t, &tick, PixOpAdd());
            threshold(&color, &alpha, Color(0), Color((i + 1)), 0);
            invert(&alpha, &alpha);
            copy(&maskimg, {0, 0},    &color, ChanShuf(n, 1, 0, 1) | PixOpAdd());
            copy(&maskimg, {size, 0}, &alpha, ChanShuf(n, 1, 0, 1) | PixOpMin());

            angle += angle_step;
        }

        angle_shift += angle_shift_step;
    }

    for(int n=0; n<4; n++)
    {
        Image img(size, size, 2);
        fill(&img, Color(0, 255));
        copy(&img, {0, 0}, {&maskimg, {0, 0, size, size}},    ChanShuf(0, 1, n, 1));
        copy(&img, {0, 0}, {&maskimg, {size, 0, size, size}}, ChanShuf(1, 1, n, 1));
        p->putImage(&img, {100, 100});
        p->putImage(&img, {110 + size + (size + 10) * n, 100});
    }

    unsigned int val = 72;
    unsigned char res_color[4] = {255, 0, 0, 0};
    Image resimg(size, size, 4);
    fill(&resimg, Color(0, 0, 0, 255));
    for(int y=0; y<resimg.height(); y++)
    {
        for(int x=0; x<resimg.width(); x++)
        {
            auto colorpx = maskimg.pixel(x, y);
            auto alphapx = maskimg.pixel(x + size, y);

            int nlayers = 0;
            int layer = 0;
            for(int m=0; m<4; m++)
            {
                unsigned int imgval = colorpx[m];
                imgval = (imgval << 2) + m;
                if(imgval >= val)
                {
                    nlayers++;
                    layer = m;
                }
            }


            unsigned char alpha = 255;
            if(nlayers > 0)
                alpha = (nlayers == 1 ? alphapx[layer] : 0);

            resimg(x, y)[0] = res_color[0];
            resimg(x, y)[1] = res_color[1];
            resimg(x, y)[2] = res_color[2];
            resimg(x, y)[3] = alpha;
        }
    }

//     Image ring(size, size, 1);
//     fill(
//         &ring, 0, 1, 255
//     );
//     fill_circle(
//         &ring, 0, 1, Color(0), {0, 0}, size
//     );
//     fill_circle(
//         &ring, 0, 1, Color(255), {3, 3}, size - 6
//     );
//     copy(&resimg, {0, 0}, &ring, ChanShuf(3, 1, 0, 1) | PixOpMul());

    p->putImage(&resimg, {100, 330});

//     int size = 50;
//     Image src(size*2, size, 4);
//     for(int y=0; y<src.height(); y++)
//     {
//         for(int x=0; x<src.width(); x++)
//         {
//             unsigned char color[4];
//             color[0] = 0;
//             color[1] = 0;
//             color[2] = 0;
//             color[3] = 0;
// 
//             if((x & 1) ^ (y & 1))
//                 color[x<size ? 0 : 1] = 255;
// 
//             for(int c=0; c<4; c++)
//             {
//                 src(x, y)[c] = color[c];
//             }
//         }
//     }
// 
//     p->putImage(&src, {100, 100});
// 
//     for(int n=0; n<2; n++)
//     {
//         Image dst(size, size, 4);
//         copy(&dst, {0, 0}, {&src, {(n & 1) ? size : 0, 0, size, size}}, PixOpReplace());
//         p->putImage(&dst, {100 + size * n, 160});
//     }
}


void View_Project::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        m_angle += event->dy() * 0.025f;
        while(m_angle < 0.0f)
            m_angle += M_PI * 2.0f;
        while(m_angle > (M_PI * 2.0f))
            m_angle -= M_PI * 2.0f;
        repaint();
    }

    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}


}//namespace r64fx
