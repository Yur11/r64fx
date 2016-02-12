#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "Widget_Container.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"

#include "KeyEvent.hpp"

using namespace std;
using namespace r64fx;


// void gen_knob(Image* dst, float ang)
// {
//     unsigned char bg = 0;
//
//     float w = dst->width();
//     float h = dst->height();
//     float hw = w * 0.5f;
//     float hh = h * 0.5f;
//
//     Image src(hw, 3, 1);
//     fill(&src, 255);
//     src(0,               0)[0] =
//     src(0,               2)[0] =
//     src(src.width() - 1, 0)[0] =
//     src(src.width() - 1, 2)[0] =
//     127;
//
//     Transform2D<float> transform;
//     transform.translate(hw, hh - 1);
//     transform.rotate(ang);
//     transform.translate(1, -1);
//
//     bilinear_copy(
//         dst, &src, {0, 0, int(w), int(h)},
//         transform, BilinearCopyMode::AddWithSaturation,
//         &bg, 1
//     );
// }



void draw_radius(Image* dst, Point<float> center, float radius, float angle, float thickness)
{
    unsigned char fg = 255;
    unsigned char bg = 0;

    Point<float> b(
        cos(angle) * radius + center.x(),
        sin(angle) * radius + center.y()
    );
    draw_line(dst, center, b, thickness, LineCapStyle::Round, &fg, &bg, 1);
}


void draw_circle(Image* dst, Point<float> center, float radius, float thickness, int npoints)
{
    unsigned char fg = 255;
    unsigned char bg = 0;

    float npoints_rcp = 1.0f / float(npoints);
    float arc_step = (2.0f * M_PI) * npoints_rcp;
    float arc = 0.0f;
    vector<Point<float>> points;
    for(int i=0; i<npoints; i++)
    {
        float x = cos(arc) * radius + center.x();
        float y = sin(arc) * radius + center.y();
        points.push_back({x, y});
        arc += arc_step;
    }
    points.push_back(points[0]);
    draw_lines(dst, points.data(), points.size(), thickness, LineCapStyle::Round, &fg, &bg, 1);
}


void draw_arc(Image* dst, Point<float> center, float radius, float thickness, float ang_a, float ang_b, int npoints)
{
    unsigned char fg = 255;
    unsigned char bg = 0;

    float npoints_rcp = 1.0f / float(npoints);
    float ang_delta = ang_b - ang_a;
    float arc_step = ang_delta * npoints_rcp;
    float arc = ang_a;
    vector<Point<float>> points;
    for(int i=0; i<=npoints; i++)
    {
        float x = cos(arc) * radius + center.x();
        float y = sin(arc) * radius + center.y();
        points.push_back({x, y});
        arc += arc_step;
    }
    draw_lines(dst, points.data(), points.size(), thickness, LineCapStyle::Round, &fg, &bg, 1);
}


class MyWidget : public Widget{
    Image  m_Image;
    float  m_ang = M_PI * 0.75f;
    int    m_size = 64;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {

    }

    ~MyWidget()
    {
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        m_Image.load(width(), height(), 4);
        for(int y=0; y<m_Image.height(); y++)
        {
            for(int x=0; x<m_Image.width(); x++)
            {
                unsigned char px[4] = { 127, 180, 255, 0 };
                m_Image.setPixel(x, y, px);
            }
        }

        unsigned char fg = 255;
        unsigned char bg = 0;

        Image dst(m_size, m_size, 1);
        fill(&dst, 0);
        Point<float> center(float(dst.width()/2), float(dst.height()/2));
        float radius = dst.width()/2 - 5;
        draw_radius(&dst, center, radius, m_ang, 3);
        draw_arc(&dst, center, radius, 3, M_PI * 0.75f, M_PI * 2.25f, m_size / 3 + 10);

        alpha_blend(&m_Image, {10,               10},                {0,   0,   0,   0}, &dst);
        alpha_blend(&m_Image, {10 + dst.width(), 10},                {200, 50,  50,  0}, &dst);
        alpha_blend(&m_Image, {10 + dst.width(), 10 + dst.height()}, {50,  100, 50,  0}, &dst);
        alpha_blend(&m_Image, {10,               10 + dst.height()}, {50,  50,  200, 0}, &dst);

        auto painter = event->painter();
        painter->putImage(&m_Image);
        Widget::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget::mousePressEvent(event);
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        const float step = 0.05;

        if(event->key() == Keyboard::Key::Up)
        {
            if(m_ang <= (M_PI * 2.25f - step))
                m_ang += step;
//             if(m_ang >= (M_PI * 2))
//                 m_ang -= (M_PI * 2);
        }
        else if(event->key() == Keyboard::Key::Down)
        {
            if(m_ang >= (M_PI * 0.75f + step))
                m_ang -= step;
//             if(m_ang < 0)
//                 m_ang -= (M_PI * 2);
        }
        else if(event->key() == Keyboard::Key::Left)
        {
            if(m_size > 10)
            {
                m_size--;
                cout << m_size << "\n";
            }
        }
        else if(event->key() == Keyboard::Key::Right)
        {
            m_size++;
            cout << m_size << "\n";
        }

        update();
    }

    virtual void closeEvent()
    {
        Program::quit();
    }
};


class MyProgram : public Program{
    Font*   m_Font = nullptr;
    Widget_Container* m_container = nullptr;

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
//         m_Widget = new MyWidget;
//         m_Widget->setSize({1000, 600});
//         m_Widget->show();

        m_Font = new Font("", 20, 72);

        m_container = new Widget_Container;
        {
            auto mw = new MyWidget(m_container);
            mw->setWidth(300);
            mw->setHeight(300);
        }

        for(int i=0; i<1; i++)
        {
            auto wt = new Widget_Text("", m_Font, m_container);
            wt->setWidth(300);
            wt->setHeight(300);
            wt->setPadding(5);
            wt->setTextWrap(TextWrap::Anywhere);
            wt->setTextAlignment(TextAlignment::Left);

        }
        m_container->setPadding(5);
        m_container->setSpacing(5);
        m_container->alignHorizontally();
        m_container->show();
    }
    
    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        if(m_Font)
            delete m_Font;

        if(m_container)
            delete m_container;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
