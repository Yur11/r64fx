#include "FilterView.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "TextPainter.hpp"
#include "Complex.hpp"
#include <vector>
#include <iostream>

using namespace std;

namespace r64fx{

namespace{

constexpr int handle_size = 15;

Image* g_cross_img   = nullptr;
Image* g_circle_img  = nullptr;

Font* g_Font = nullptr;

unsigned long g_instance_count = 0;

void init()
{
    if(g_instance_count == 0)
    {
        g_cross_img = new Image(handle_size, handle_size);
        fill(g_cross_img, Color(0));
        {
            int img_size = handle_size;
            Image img(img_size, img_size);
            fill(&img, Color(0));
            fill(&img, Color(255), {0, img_size/2, img_size, 1});
            fill(&img, Color(255), {img_size/2, 0, 1, img_size});

            Transform2D<float> t;
            t.translate(img_size/2, img_size/2);
            t.rotate(M_PI/4);
            t.translate(-img_size/2, -img_size/2);

            implant(g_cross_img, t, &img);
        }

        g_circle_img = new Image(handle_size, handle_size);
        fill(g_circle_img, Color(0));
        stroke_circle(
            g_circle_img, Color(255), {handle_size * 0.5f - 0.5f, handle_size * 0.5f - 0.5f}, handle_size * 0.5f - 1.0f, 1.0f
        );

        g_Font = new Font("", 15, 72);
    }
    g_instance_count++;
}

void cleanup()
{
    g_instance_count--;
    if(g_instance_count == 0)
    {
        if(g_cross_img)
        {
            delete g_cross_img;
        }

        if(g_circle_img)
        {
            delete g_circle_img;
        }

        if(g_Font)
        {
            delete g_Font;
        }
    }
}

class PoleZeroPlot;
class ResponsePlot;

}//namespace


struct FilterViewPrivate{
    FilterViewControllerIface* ctrl = nullptr;

    PoleZeroPlot* pole_zero_plot = nullptr;
    ResponsePlot* response_plot  = nullptr;

    FilterViewPrivate()
    {
        init();
    }

    ~FilterViewPrivate()
    {
        cleanup();
    }

    void updatePlots();
};


namespace{

class PoleZeroPlot : public Widget{
    FilterViewPrivate* m = nullptr;

    Image markup_img;
    Image zimg;

    vector<Complex<float>> zeros;
    vector<Complex<float>> poles;
    Complex<float>* current_point = nullptr;
    Point<int> handle_anchor = {0, 0};

public:
    PoleZeroPlot(FilterViewPrivate* m, Widget* parent = nullptr)
    : Widget(parent)
    , m(m)
    {
        zeros.push_back({1.0f, 0.0f});
//         zeros.push_back({1.0f, 0.0f});
//         zeros.push_back({1.0f, 0.0f});
//         zeros.push_back({1.0f, 0.0f});
//         zeros.push_back({1.0f, 0.0f});
//         
//         poles.push_back({0.0f, 0.7f});
        poles.push_back({0.0f, 0.7f});
//         poles.push_back({0.0f, 0.7f});
//         poles.push_back({0.0f, 0.7f});
//         poles.push_back({0.0f, 0.7f});
//         poles.push_back({0.0f, 0.7f});
    }

    virtual ~PoleZeroPlot()
    {

    }

    Complex<float> evaluateAt(Complex<float> z)
    {
        Complex<float> numerator(1, 0);
        for(auto zero : zeros)
        {
            numerator *= (z - zero);
            if(zero.im != 0.0f)
            {
                numerator *= (z - zero.conjugate());
            }
        }

        Complex<float> denominator(1, 0);
        for(auto pole : poles)
        {
            denominator *= (z - pole);
            if(pole.im != 0.0f)
            {
                denominator *= (z - pole.conjugate());
            }
        }

        if(denominator.magnitude() != 0)
            return numerator / denominator;
        else
            return denominator;
    }

    void update()
    {
        zimg.load(width(), height(), 1);

        for(int y=0; y<height(); y++)
        {
            for(int x=0; x<width(); x++)
            {
                auto z = pointToComplex({x, y});
                auto val = evaluateAt(z);
                unsigned char px = 255;
                float mag = val.magnitude();
                if(mag < 500.0f)
                {
                    mag /= 500.0f;
                    mag = sqrt(mag);
                    mag *= 255.0f;
                    px = (unsigned char)mag;
                }
                zimg.pixel(x, y)[0] = px;

            }
        }

        markup_img.load(width(), height(), 1);
        fill(&markup_img, Color(0));

        stroke_circle(
            &markup_img, Color(255), {width()*0.5f, height()*0.5f}, min(width(), height()) * 0.45, 1.0f
        );

        fill(&markup_img, Color(255), {0, height()/2 - 1, width(), 1});
        {
            Image* textimg = text2image("Re", TextWrap::None, g_Font);
            implant(&markup_img, Point<int>(width() - textimg->width() - 2,  height()/2 - textimg->height() - 1), textimg);
            delete textimg;
        }

        fill(&markup_img, Color(255), {width()/2 - 1, 0, 1, height()});
        {
            Image* textimg = text2image("Im", TextWrap::None, g_Font);
            implant(&markup_img, Point<int>(width()/2 + 3, 1), textimg);
            delete textimg;
        }
    }

private:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();

        p->fillRect({0, 0, width(), height()}, Color(0, 0, 0, 0));
        if(zimg.isGood())
        {
            p->blendColors({0, 0}, Colors(Color(255, 255, 255, 0)), &zimg);
        }

        if(markup_img.isGood())
        {
            p->blendColors({0, 0}, Colors(Color(0, 127, 0, 0)), &markup_img);
        }

        Point<int> handle_offset((handle_size >> 1) + 1, (handle_size >> 1) + 1);

        for(auto zero : zeros)
        {
            p->blendColors(
                complexToPoint(zero) - handle_offset,
                Colors(Color(0, 0, 255, 0)),
                g_circle_img
            );
        }

        for(auto pole : poles)
        {
            p->blendColors(
                complexToPoint(pole) - handle_offset,
                Colors(Color(255, 0, 0, 0)),
                g_cross_img
            );
        }
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        update();
        repaint();
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
//         auto val = evaluateAt(pointToComplex(event->position()));
        Point<int> anchor(0, 0);
        auto point = pointAt(event->position(), &anchor);
        if(point)
        {
            current_point = point;
            handle_anchor = anchor;
        }
    }

    virtual void mouseReleaseEvent(MouseReleaseEvent* event)
    {
        current_point = nullptr;
        handle_anchor = {0, 0};
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(current_point)
        {
            auto pos = event->position() - handle_anchor;
            current_point[0] = pointToComplex(pos);
            m->updatePlots();
            parent()->repaint();
        }
    }

    Complex<float> pointToComplex(Point<int> point) const
    {
        return {
            +float((point.x() - width()/2)  / apparentRadius()),
            -float((point.y() - height()/2) / apparentRadius())
        };
    }

    Point<int> complexToPoint(Complex<float> complex) const
    {
        return{
            +int(complex.re * apparentRadius()) + width()/2,
            -int(complex.im * apparentRadius()) + height()/2
        };
    }

    float apparentRadius() const
    {
        return widthOrHeight() * 0.45f;
    }

    int widthOrHeight() const
    {
        return min(width(), height());
    }

    Complex<float>* pointAt(Point<int> pos, Point<int>* anchor_out = nullptr)
    {
        Complex<float>* result = nullptr;
        Point<int> anchor(0, 0);

        for(auto &pole : poles)
        {
            Point<int> pole_pos = complexToPoint(pole);
            Point<int> d = (pos - pole_pos);

            if(abs(d.x()) < handle_size/2 && abs(d.y()) < handle_size/2)
            {
                result = &pole;
                anchor = {d.x(), d.y()};
            }
        }

        for(auto &zero : zeros)
        {
            Point<int> zero_pos = complexToPoint(zero);
            Point<int> d = (pos - zero_pos);

            if(abs(d.x()) < handle_size/2 && abs(d.y()) < handle_size/2)
            {
                result = &zero;
                anchor = {d.x(), d.y()};
            }
        }

        if(anchor_out)
        {
            anchor_out[0] = anchor + Point<int>(handle_size/2, handle_size/2);
        }
        return result;
    }
};


class ResponsePlot : public Widget{
    FilterViewPrivate* m = nullptr;

    vector<float> response;

public:
    ResponsePlot(FilterViewPrivate* m, Widget* parent = nullptr)
    : Widget(parent)
    , m(m)
    {

    }

    ~ResponsePlot()
    {

    }

    void update()
    {
        if(width() > 0)
        {
            response.resize(width());
            float rcp = 1.0f / float(width() - 1);
            for(int i=0; i<width(); i++)
            {
                float phase = (float(i) * rcp) * M_PI;
                Complex<float> z(Polar<float>(1.0, phase));
                Complex<float> value = m->pole_zero_plot->evaluateAt(z);
                response[i] = (height() / 2) - (value.magnitude() * height() * 0.125);
            }
        }
    }

private:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(0, 0, 0, 0));
        Image img(width(), height(), 1);
        fill(&img, Color(0));
        stroke_plot(&img, Color(255), {0, 0, width(), height()}, response.data(), 2.0f, 1.0f, height() / 2);
        p->blendColors({0, 0}, Colors(Color(255, 63, 0)), &img);
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        update();
        repaint();
    }
};

}//namespace


void FilterViewPrivate::updatePlots()
{
    pole_zero_plot->update();
    response_plot->update();
}


FilterView::FilterView(FilterViewControllerIface* ctrl, Widget* parent)
: Widget(parent)
{
    m = new FilterViewPrivate;
    m->ctrl = ctrl;

    m->pole_zero_plot = new PoleZeroPlot(m, this);
    m->response_plot = new ResponsePlot(m, this);

    setSize({800, 400});
}


FilterView::~FilterView()
{
    if(m)
    {
        if(m->pole_zero_plot)
        {
            delete m->pole_zero_plot;
        }

        if(m->response_plot)
        {
            delete m->response_plot;
        }

        delete m;
    }
}


void FilterView::resizeEvent(WidgetResizeEvent* event)
{
    if(event->width() > event->height())
    {
        m->pole_zero_plot->setPosition({0, 0});
        m->pole_zero_plot->setSize({event->height(), event->height()});

        m->response_plot->setPosition({m->pole_zero_plot->width(), 0});
        m->response_plot->setSize({event->width() - m->pole_zero_plot->width(), event->height()});
    }
}

}//namespace r64fx
