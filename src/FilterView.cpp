#include "FilterView.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "TextPainter.hpp"
#include "Complex.hpp"

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

}//namespace

struct FilterViewPrivate{
    FilterViewControllerIface* ctrl = nullptr;

    Image bgimg;

    FilterViewPrivate()
    {
        init();
    }

    ~FilterViewPrivate()
    {
        cleanup();
    }
};


FilterView::FilterView(FilterViewControllerIface* ctrl, Widget* parent)
: Widget(parent)
{
    m = new FilterViewPrivate;
    m->ctrl = ctrl;

    setSize({400, 400});
}


FilterView::~FilterView()
{
    if(m)
    {
        delete m;
    }
}


void FilterView::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(200, 200, 200, 0));
    if(m->bgimg.isGood())
    {
        p->blendColors({0, 0}, Colors(Color(0, 0, 0, 0)), &(m->bgimg));
    }
    p->blendColors({100, 100}, Colors(Color(0, 0, 0, 0)), g_cross_img);
    p->blendColors({150, 100}, Colors(Color(0, 0, 0, 0)), g_circle_img);
}


void FilterView::resizeEvent(ResizeEvent* event)
{
    m->bgimg.load(event->width(), event->height(), 1);
    fill(&(m->bgimg), Color(0));

    stroke_circle(
        &(m->bgimg), Color(255), {m->bgimg.width()*0.5f, m->bgimg.height()*0.5f}, min(m->bgimg.width(), m->bgimg.height()) * 0.45, 1.0f
    );

    fill(&(m->bgimg), Color(255), {0, m->bgimg.height()/2 - 1, m->bgimg.width(), 1});
    {
        Image* textimg = text2image("Re", TextWrap::None, g_Font);
        implant(&(m->bgimg), Point<int>(m->bgimg.width() - textimg->width() - 2,  m->bgimg.height()/2 - textimg->height() - 1), textimg);
        delete textimg;
    }

    fill(&(m->bgimg), Color(255), {m->bgimg.width()/2 - 1, 0, 1, m->bgimg.height()});
    {
        Image* textimg = text2image("Im", TextWrap::None, g_Font);
        implant(&(m->bgimg), Point<int>(m->bgimg.width()/2 + 3, 1), textimg);
        delete textimg;
    }
}

}//namespace r64fx