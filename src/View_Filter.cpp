#include "View_Filter.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "TextPainter.hpp"
#include "Complex.hpp"
#include <vector>
#include <iostream>

#include "FilterClass.hpp"

using namespace std;

namespace r64fx{

namespace{

constexpr int handle_size = 15;

Image* g_cross_img   = nullptr;
Image* g_circle_img  = nullptr;
Point<int> g_handle_offset;

Font* g_Font = nullptr;

unsigned long g_instance_count = 0;

constexpr float g_apparent_unity = 0.9;

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
            fill({&img, {0, img_size/2, img_size, 1}}, Color(255));
            fill({&img, {img_size/2, 0, 1, img_size}}, Color(255));

            Transformation2D<float> t;
            t.translate(img_size/2, img_size/2);
            t.rotate(M_PI/4);
            t.translate(-img_size/2, -img_size/2);

            copy(g_cross_img, t, &img);
        }

        g_circle_img = new Image(handle_size, handle_size);
        fill(g_circle_img, Color(0));
        fill_circle(g_circle_img, 0, 1, Color(255), {handle_size >> 1, handle_size >> 1}, (handle_size >> 1) - 1);
        fill_circle(g_circle_img, 0, 1, Color(0),   {handle_size >> 1, handle_size >> 1}, (handle_size >> 1) - 2);

        g_handle_offset = Point<int>(handle_size >> 1, handle_size >> 1);

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


void load_sys_fun_to_buffer(FilterClass* fc, Complex<float>* buff, int buff_capacity)
{
#ifdef R64FX_DEBUG
    assert(fc->rootBufferSize() <= buff_capacity);
#endif//R64FX_DEBUG

    if(fc->isEmpty())
        return;

    int n = 0;
    for(auto root : fc->roots())
    {
#ifdef R64FX_DEBUG
        assert(root->hasValue());
#endif//R64FX_DEBUG
        buff[n] = root->value();
        n++;
        if(root->hasConjugate())
        {
            buff[n] = root->value().conjugate();
            n++;
        }
    }
}

class PoleZeroPlot;
class ResponsePlot;

}//namespace


struct View_FilterPrivate{
    View_FilterControllerIface* ctrl = nullptr;

    FilterClass* fc = nullptr;

    PoleZeroPlot* pole_zero_plot = nullptr;
    ResponsePlot* response_plot  = nullptr;

    View_FilterPrivate()
    {
        init();
    }

    ~View_FilterPrivate()
    {
        cleanup();
    }
};


namespace{

class PoleZeroPlot : public Widget{
    View_FilterPrivate* m = nullptr;

    Image markup_img;

    SysFunRoot* m_selected_root = nullptr;
    Point<int> m_handle_anchor = {0, 0};

    PainterTexture1D* m_pole_zero_texture = nullptr;

    /* Calculated once on update & used in subsequent repaints.*/
    int m_zero_count = 0;
    int m_pole_count = 0;

    float m_apparent_radius      = 0.0f;    // Radius of the unit circle in pixels;
    float m_apparent_radius_rcp  = 0.0f;
    float m_sys_fun_img_radius   = 0.0f;    // Size of X or O image in sys. fun. coords.

public:
    PoleZeroPlot(View_FilterPrivate* m, Widget* parent = nullptr)
    : Widget(parent)
    , m(m)
    {
    }

    virtual ~PoleZeroPlot()
    {

    }

    void update()
    {
        m_apparent_radius = float(width() < height() ? width() : height()) * g_apparent_unity * 0.5f;
        m_apparent_radius_rcp = 1.0f / m_apparent_radius;
        m_sys_fun_img_radius = float(handle_size) * m_apparent_radius_rcp * 0.5f;

        updateSysFunTexture();

        markup_img.load(width(), height(), 1);
        fill(&markup_img, Color(0));

        fill_circle(&markup_img, 0, 1, Color(255), {width() >> 1, height() >> 1}, (min(width(), height()) * 0.45) + 1);
        fill_circle(&markup_img, 0, 1, Color(0),   {width() >> 1, height() >> 1}, (min(width(), height()) * 0.45));

        fill({&markup_img, {0, height()/2 - 1, width(), 1}}, Color(255));
//         {
//             Image* textimg = text2image("Re", TextWrap::None, g_Font);
//             copy({&markup_img, Point<int>(width() - textimg->width() - 2,  height()/2 - textimg->height() - 1)}, textimg);
//             delete textimg;
//         }

        fill({&markup_img, {width()/2 - 1, 0, 1, height()}}, Color(255));
//         {
//             Image* textimg = text2image("Im", TextWrap::None, g_Font);
//             copy({&markup_img, Point<int>(width()/2 + 3, 1)}, textimg);
//             delete textimg;
//         }
    }


    void updateSysFunTexture()
    {
        if(m->fc)
        {
            m_zero_count = m->fc->zeroBufferSize();
            m_pole_count = m->fc->poleBufferSize();
            int buff_len = m_zero_count + m_pole_count;
            if(buff_len > 0)
            {
                Complex<float> buff[32];
                load_sys_fun_to_buffer(m->fc, buff, 32);
                for(int n=0; n<buff_len; n++)
                {
                    auto &val = buff[n];
                    val *= Complex<float>(g_apparent_unity * 0.5f, 0.0f);
                    val += Complex<float>(0.5f, 0.5f);
                }
                m_pole_zero_texture->load((float*)buff, buff_len, 2);
            }
        }
    }

private:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();

        if(m->fc)
        {
            if(m_pole_zero_texture && m_pole_zero_texture->isGood())
            {
                p->drawPoleZeroPlot({0, 0, width(), height()}, m_pole_zero_texture, 0, m_zero_count, m_zero_count, m_pole_count);
            }
            else
            {
                p->fillRect({0, 0, width(), height()}, Color(0));
            }

            if(markup_img.isGood())
            {
                p->blendColors({0, 0}, Colors(Color(0, 127, 0, 0)), &markup_img);
            }

            for(auto root : m->fc->roots())
            {
#ifdef R64FX_DEBUG
                assert(root->hasValue());
#endif//R64FX_DEBUG
                auto val = root->value();

                auto img = (root->isPole() ? g_cross_img : g_circle_img);
                Color color = (root->isPole() ? Color(255, 0, 0) : Color(0, 0, 255));

                p->blendColors(complexToPoint(val) - g_handle_offset, color, img);
                if(root->hasConjugate())
                    p->blendColors(complexToPoint(val.conjugate()) - g_handle_offset, color, img);
            }
        }
    }

    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event)
    {
#ifdef R64FX_DEBUG
        assert(m_pole_zero_texture == nullptr);
#endif//R64FX_DEBUG
        m_pole_zero_texture = event->textureManager()->newTexture();
        update();
    }

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
    {
#ifdef R64FX_DEBUG
        assert(m_pole_zero_texture != nullptr);
#endif//R64FX_DEBUG
        event->textureManager()->deleteTexture(m_pole_zero_texture);
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        update();
        repaint();
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Point<int> anchor(0, 0);
        auto root = rootAt(event->position());
        if(root)
        {
            m_selected_root = root;
            grabMouseFocus();
        }
    }

    virtual void mouseReleaseEvent(MouseReleaseEvent* event)
    {
        m_selected_root = nullptr;
        releaseMouseFocus();
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(m_selected_root)
        {
            auto z = pointToComplex(event->position());
            if(!m_selected_root->hasConjugate())
                z.im = 0.0f;
            if(m_selected_root->isPole())
            {
                if(z.magnitude() >= 0.99999f)
                    z.setMagnitude(0.99999f);
            }
            m_selected_root->setValue(z);
            updateSysFunTexture();
            updateResponsePlot();
            parent()->repaint();
        }
    }

    void updateResponsePlot();


    Complex<float> pointToComplex(Point<int> point) const
    {
        return {
            +float((point.x() - width()/2)  * m_apparent_radius_rcp),
            -float((point.y() - height()/2) * m_apparent_radius_rcp)
        };
    }

    Point<int> complexToPoint(Complex<float> complex) const
    {
        return{
            +int(complex.re * m_apparent_radius) + width()/2,
            -int(complex.im * m_apparent_radius) + height()/2
        };
    }

    SysFunRoot* rootAt(Point<int> pos)
    {
        if(!m->fc)
            return nullptr;

        auto z = pointToComplex(pos);
        auto s = m_sys_fun_img_radius;
        auto l = z.re - s;
        auto r = z.re + s;
        auto t = z.im - s;
        auto b = z.im + s;

        for(auto root : m->fc->roots())
        {
#ifdef R64FX_DEBUG
            assert(root->hasValue());
#endif//R64FX_DEBUG
            auto v = root->value();

            if(v.re < r && v.re > l && v.im < b && v.im > t)
            {
                return root;
            }

            if(root->hasConjugate())
            {
                v = v.conjugate();
                if(v.re < r && v.re > l && v.im < b && v.im > t)
                {
                    return root;
                }
            }
        }

        return nullptr;
    }
};


class ResponsePlot : public Widget{
    View_FilterPrivate* m = nullptr;

    vector<float> response;

public:
    ResponsePlot(View_FilterPrivate* m, Widget* parent = nullptr)
    : Widget(parent)
    , m(m)
    {

    }

    ~ResponsePlot()
    {

    }

    void update()
    {
        if(!m->fc)
            return;

        if(width() > 0)
        {
            response.resize(width());
            float rcp = 1.0f / float(width() - 1);
            for(int i=0; i<width(); i++)
            {
                float phase = (float(i) * rcp) * M_PI;
                Complex<float> z(Polar<float>(1.0, phase));
                Complex<float> value = m->fc->evalAt(z);
                response[i] = (height() / 2) - (value.magnitude() * height() * 0.125f * 0.125f);
//                 response[i] = (value.phase() / M_PI) * height() * 0.5f;
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
        stroke_plot(&img, Color(255), {0, 0, width(), height()}, response.data(), 1.0f, 1.0f, height() / 2);
        p->blendColors({0, 0}, Color(255, 63, 0), &img);

//         for(int x=0; x<width(); x++)
//         {
//             if((x & 15) == 0)
//                 p->fillRect({x, 0, 1, height()}, Color(63, 63, 63));
//         }
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        update();
        repaint();
    }
};


void PoleZeroPlot::updateResponsePlot()
{
    m->response_plot->update();
}

}//namespace


View_Filter::View_Filter(View_FilterControllerIface* ctrl, Widget* parent)
: Widget(parent)
{
    m = new View_FilterPrivate;
    m->ctrl = ctrl;

    m->pole_zero_plot = new PoleZeroPlot(m, this);
    m->response_plot = new ResponsePlot(m, this);

    setSize({800, 400});

    //Remove This!
    auto fc = new FilterClass;
//     fc->newRoot<Pole>({-0.25f, 0.25f});
    fc->newRoot<Pole>({0.5f, 0.5f});
//     fc->newRoot<Pole>({0.5f, 0.5f});
    fc->newRoot<Zero>({-0.5f, 0.5f});
//     fc->newRoot<Zero>({-0.25f, 0.25f});
//     fc->newRoot<Zero>({-0.25f, 0.25f});
//     fc->newRoot<Zero>({-0.25f, 0.25f});
//     fc->newRoot<Zero>({-0.25f, 0.25f});
    fc->updateIndices();
    setFilterClass(fc);
}


View_Filter::~View_Filter()
{
    if(m)
    {
        delete m->fc;//Remove This!

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


void View_Filter::setFilterClass(FilterClass* fc)
{
    m->fc = fc;
}


void View_Filter::resizeEvent(WidgetResizeEvent* event)
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
