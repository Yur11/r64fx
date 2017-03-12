#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

enum class KnobDecoration{
    SolidHorseShoe,
    DashedHorseShoe
};


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

    void genKnob(Image* dst, Point<int> dstpos, KnobDecoration kd)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill({dst, {dstpos.x(), dstpos.y(), m_size, m_size}}, Color(31, 255));

        Image alpha_mask(m_size, m_size, 1);
        fill(&alpha_mask, 0, 1, 255);

        switch(kd)
        {
            case KnobDecoration::SolidHorseShoe:
            {
                genDecorationSolidHorseShoe(&alpha_mask, dstpos);
                break;
            }

            case KnobDecoration::DashedHorseShoe:
            {
                genDecorationDashedHorseShoe(&alpha_mask, dstpos, 10);
                break;
            }

            default:
                break;
        }

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

    void genDecorationSolidHorseShoe(Image* alpha_mask, Point<int> dstpos)
    {
        Image horse_shoe(m_size, m_size, 1);
        fill(&horse_shoe, 0, 1, 255);

        fill_circle(
            &horse_shoe, 0, 1, Color(0), {0, 0}, m_size
        );
        fill_circle(
            &horse_shoe, 0, 1, Color(255), {3, 3}, m_size - 6
        );
        fill_bottom_triangle(
            &horse_shoe, 1, 1, Color(255), {0, 0}, m_size
        );

        Image tick(m_size, m_size, 1);
        fill(&tick, 0, 1, 255);
        fill({&tick, {m_size/2 - 1, 2, 3, 10}}, 0, 1, 0);
        copyTick(&horse_shoe, &tick, -M_PI * 0.75f);

        mirror_left2right(&horse_shoe);
        copy(alpha_mask, {0, 0}, &horse_shoe, PixOpMin());

        fill({alpha_mask, {halfSize() - 2, 0, 4, 6}}, 0, 1, 255);
        fill({alpha_mask, {halfSize() - 1, 0, 2, 5}}, 0, 1, 0);
    }

    void genDecorationDashedHorseShoe(Image* alpha_mask, Point<int> dstpos, int nticks)
    {
        Image horse_shoe(m_size, m_size, 1);
        fill(&horse_shoe, 0, 1, 255);

        Image long_tick(m_size, m_size, 1);
        fill(&long_tick, 0, 1, 255);
        fill({&long_tick, {m_size/2 - 1, 2, 2, 4}}, 0, 1, 0);

        Image short_tick(m_size, m_size, 1);
        fill(&short_tick, 0, 1, 255);
        fill({&short_tick, {m_size/2 - 1, 2, 2, 4}}, 0, 1, 0);

        float angle_range = M_PI * 0.75;
        float angle = -angle_range;
        float angle_step = angle_range / float(nticks);

        for(int i=0; i<nticks; i++)
        {
            copyTick(&horse_shoe, i > 0 ? &short_tick : &long_tick, angle);
            angle += angle_step;
        }
        copy(&horse_shoe, {0, 0}, &long_tick, PixOpMin());

        mirror_left2right(&horse_shoe);
        copy(alpha_mask, {0, 0}, &horse_shoe, PixOpMin());
    }

    void copyTick(Image* horse_shoe, Image* tick, float angle)
    {
        Transformation2D<float> t;
        t.translate(+halfSize() - 0.5f, +halfSize() - 0.5f);
        t.rotate(angle);
        t.translate(-halfSize() + 0.5f, -halfSize() + 0.5f);
        copy(horse_shoe, t, tick, PixOpMin());
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

    for(int i=0; i<13; i++)
    {
        int size = 40 + i * 2;

        KnobAnimGenerator kanimg(size);

        Image knob(size, size, 2);
        kanimg.genKnob(&knob, {0, 0}, (i & 1) ? KnobDecoration::SolidHorseShoe : KnobDecoration::DashedHorseShoe);
        p->putImage(&knob, {10 + (i * size + 20), 20});

        Image marker(size, size, 2);
        kanimg.genMarker(&marker, {0, 0}, m_angle);
        p->putImage(&marker, {10 + (i * size + 20), 20});
    }

    Image gradimg(50, 50, 1);
    fill(&gradimg, Color(0));
    fill_gradient_radial(&gradimg, 0, 1, 0, 255, {0, 0, 50, 50});

    Image circleimg(50, 50, 1);
    fill(&circleimg, Color(0));
    fill_circle(&circleimg, 0, 1, Color(255), {0, 0}, 50);
    copy(&gradimg, {0, 0}, &circleimg, PixOpMul());

    Image img(50, 50, 4);
    fill(&img, Color(0, 0, 0, 0));

    threshold(&img, 0, 4, {0, 0}, Color(255, 0, 0, 0), Color(255, 0, 0, 255), &gradimg, 255.0f * ((normalize_angle(m_angle - M_PI * 0.5f) / (2.0f * M_PI))));
    flip_hori(&img);
    p->putImage(&img, {100, 100});

    Image tick(50, 50, 4);
    fill(&tick, Color(255, 0, 0, 255));
    fill({&tick, {23, 0, 4, 27}}, Color(255, 0, 0, 0));
    Image tickimg(50, 50, 4);
    fill(&tickimg, Color(0, 0, 0, 255));
    Transformation2D<float> t;
    t.translate(float(25) - 0.5f, float(25) - 0.5f);
    t.rotate(-(M_PI * 0.5f + m_angle));
    t.translate(float(-25) + 0.5f, float(-25) + 0.5f);
    copy(&tickimg, t, &tick, PixOpReplace());

    p->putImage(&tickimg, {100, 100});

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
