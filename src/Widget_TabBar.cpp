#include "Widget_TabBar.hpp"
#include "WidgetFlags.hpp"
#include "LayoutUtils.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
Font* g_tab_bar_font = nullptr;

void init_tab_bar_font_if_needed()
{
    if(g_tab_bar_font)
        return;

    g_tab_bar_font = new Font("", 14, 72);
}


class TabHandle : public Widget{
    Image m_img;
    std::string m_caption = nullptr;
    void* m_payload = nullptr;
    
public:
    TabHandle(Widget* parent, const std::string &caption, void* tab_payload) 
    : Widget(parent)
    , m_caption(caption)
    , m_payload(tab_payload)
    {
        resizeAndRealign();
    }
    
    virtual ~TabHandle()
    {
        
    }
    
    void resizeAndRealign()
    {
        if(m_caption.empty())
        {
            m_img.free();
        }
        else
        {
            text2image(m_caption, TextWrap::None, g_tab_bar_font, &m_img);
            setSize({m_img.width(), m_img.height() + 10});
        }
    }
    
    inline std::string caption() const
    {
        return m_caption;
    }
    
    inline void* payload() const
    {
        return m_payload;
    }
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        if(m_img.isGood())
        {
            p->fillRect(
                {0, 0, width(), height()}, m_flags & R64FX_TAB_SELECTED ? Color(191, 191, 191, 0) : Color(127, 127, 127, 0)
            );
            p->blendColors({0, 5}, Colors(Color(0, 0, 0, 0)), &m_img);
        }
    }
    
    virtual void mousePressEvent(MousePressEvent* event)
    {
        m_flags |= R64FX_TAB_SELECTED;
        repaint();
    }
    
    virtual void mouseReleaseEvent(MouseReleaseEvent* event)
    {
        m_flags &= ~R64FX_TAB_SELECTED;
        repaint();
    }
};
    
    
Widget_TabBar::Widget_TabBar(Widget* parent) : Widget(parent)
{
    init_tab_bar_font_if_needed();
}


Widget_TabBar::~Widget_TabBar()
{
    
}


TabHandle* Widget_TabBar::addTab(void* tab_payload, const std::string &caption)
{
    auto tab_handle = new TabHandle(this, caption, tab_payload);
    return tab_handle;
}


void Widget_TabBar::resizeAndRealign()
{
    auto s = align_horizontally(begin(), end(), {5, 0}, 10);
    setSize({s.width() + 10, s.height()});
}


void Widget_TabBar::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(255, 127, 0));
    Widget::paintEvent(event);
}


void Widget_TabBar::tabSelected(TabHandle* tab, void* tab_payload)
{
    
}
    
}//namespace r64fx
