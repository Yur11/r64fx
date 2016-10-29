#include "Widget_Label.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"

namespace r64fx{

namespace{
    Font* g_label_font = nullptr;
    int   g_label_count = 0;

    void init()
    {
        g_label_font = new Font("", 14, 72);
    }

    void init_if_needed()
    {
        if(g_label_count == 0)
        {
            init();
        }
        g_label_count++;
    }

    void cleanup()
    {
        if(g_label_font)
        {
            delete g_label_font;
            g_label_font = nullptr;
        }
    }
}//namespace


Widget_Label::Widget_Label(Widget* parent)
: Widget(parent)
{
    init_if_needed();
}


Widget_Label::Widget_Label(Image* image, bool copy, Widget* parent)
: Widget(parent)
{
    init_if_needed();
    setImage(image, copy);
}


Widget_Label::Widget_Label(const std::string &text, Widget* parent)
: Widget(parent)
{
    init_if_needed();
    setText(text);
}


Widget_Label::~Widget_Label()
{
    clear();

    g_label_count--;
    if(g_label_count == 0)
    {
        cleanup();
    }
}


void Widget_Label::setImage(Image* image, bool copy)
{
    clear();

    if(copy)
    {
        m_image = new Image(image->width(), image->height(), image->componentCount(), image->data(), true);
        m_flags |= R64FX_WIDGET_LABEL_OWNS_IMAGE;
    }
    else
    {
        m_image = image;
        m_flags &= ~R64FX_WIDGET_LABEL_OWNS_IMAGE;
    }

    if(m_image)
    {
        setSize({m_image->width(), m_image->height()});
    }
}


void Widget_Label::setText(const std::string &text)
{
    clear();

    m_image = text2image(text, TextWrap::None, g_label_font);
    if(m_image)
    {
        m_flags |= R64FX_WIDGET_LABEL_OWNS_IMAGE;
        setSize({m_image->width(), m_image->height()});
    }
}


void Widget_Label::clear()
{
    if(m_image && ownsImage())
    {
        delete m_image;
    }
    m_image = nullptr;
}


Image* Widget_Label::image() const
{
    return m_image;
}


bool Widget_Label::ownsImage()
{
    return m_flags & R64FX_WIDGET_LABEL_OWNS_IMAGE;
}


void Widget_Label::paintEvent(WidgetPaintEvent* event)
{
    if(!m_image)
        return;

    auto p = event->painter();

    if(m_image->componentCount() == 1)
    {
        unsigned char bg[4] = {200, 200, 200, 0};
        unsigned char fg[4] = {0, 0, 0, 0};
        unsigned char* colors = fg;

        p->fillRect({0, 0, width(), height()}, bg);
//         p->blendColors({0, 0}, &colors, m_image);
    }
}

}//namespace r64fx
