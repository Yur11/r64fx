#include "Widget_TabBar.hpp"
#include "WidgetFlags.hpp"
#include "LayoutUtils.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

unsigned long g_tab_bar_count = 0;
    
Font* g_tab_bar_font = nullptr;

int g_vert_padding = 5;
int g_hori_padding = 5;

void init()
{
    g_tab_bar_font = new Font("", 14, 72);
}

void tab_selected_stub(TabHandle* handle, void* payload, void* arg) {}

}//namespace


void cleanup()
{
    delete g_tab_bar_font;
    g_tab_bar_font = nullptr;
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
            setSize({
                g_hori_padding + m_img.width()  + g_hori_padding, 
                g_vert_padding + m_img.height() + g_vert_padding
            });
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
    
    void repaintSelected()
    {
        m_flags |= R64FX_TAB_SELECTED;
        repaint();
    }
    
    void repaintDeselected()
    {
        m_flags &= ~R64FX_TAB_SELECTED;
        repaint();
    }
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        if(m_img.isGood())
        {
            if(m_flags & R64FX_TAB_SELECTED)
            {
                p->fillRect({0,           0, width(), height()}, Color(191, 191, 191, 0));
                p->fillRect({0,           0, 1,       height()}, Color(175, 175, 175, 0));
                p->fillRect({width() - 1, 0, 1,       height()}, Color(175, 175, 175, 0));
            }
            else
            {
                p->fillRect({0,           0, width(), height()}, Color(127, 127, 127, 0));
                p->fillRect({0,           0, 1,       height()}, Color(127, 127, 127, 0));
                p->fillRect({width() - 1, 0, 1,       height()}, Color(111, 111, 111, 0));
            }
            
            p->blendColors({g_hori_padding, g_vert_padding}, Colors(Color(0, 0, 0, 0)), &m_img);
        }
    }
    
    virtual void mousePressEvent(MousePressEvent* event)
    {
        auto tab_bar = (Widget_TabBar*) parent();
        tab_bar->selectTab(this);
    }
    
    virtual void mouseReleaseEvent(MouseReleaseEvent* event)
    {

    }
};
    
    
Widget_TabBar::Widget_TabBar(Widget* parent) : Widget(parent)
{
    if(g_tab_bar_count == 0)
    {
        init();
    }
    g_tab_bar_count++;

    onTabSelected(nullptr);
}


Widget_TabBar::~Widget_TabBar()
{
    g_tab_bar_count--;
    if(g_tab_bar_count == 0)
    {
        cleanup();
    }
}


TabHandle* Widget_TabBar::addTab(void* tab_payload, const std::string &caption)
{
    auto tab_handle = new TabHandle(this, caption, tab_payload);
    selectTab(tab_handle);
    return tab_handle;
}


void Widget_TabBar::resizeAndRealign()
{
    auto s = align_horizontally(begin(), end(), {1, 0}, 1);
    setSize({s.width() + 2, s.height()});
}


void Widget_TabBar::onTabSelected(void (tab_selected)(TabHandle* handle, void* payload, void* arg), void* arg)
{
    if(!tab_selected)
    {
        m_tab_selected = tab_selected_stub;
    }
    else
    {
        m_tab_selected = tab_selected;
        m_tab_selected_arg = arg;
    }
}


TabHandle* Widget_TabBar::currentTab() const
{
    return m_current_tab;
}


void Widget_TabBar::selectTab(TabHandle* tab_handle)
{
    if(tab_handle->parent() == this && tab_handle != m_current_tab)
    {
        if(m_current_tab)
        {
            m_current_tab->repaintDeselected();
        }
        m_tab_selected(tab_handle, tab_handle->payload(), m_tab_selected_arg);
        
        m_current_tab = tab_handle;
        m_current_tab->repaintSelected();
    }
}


int Widget_TabBar::currentTabX() const
{
    if(!m_current_tab)
        return 0;

    return m_current_tab->x();
}


int Widget_TabBar::currentTabWidth() const
{
    if(!m_current_tab)
        return 0;

    return m_current_tab->width();
}


void Widget_TabBar::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    Widget::paintEvent(event);
    if(hasChildren())
    {
        for(auto child : *this)
        {
            auto tab_handle = dynamic_cast<TabHandle*>(child);
            if(tab_handle)
            {
                p->fillRect({tab_handle->x() - 1, 0, 1, height()}, Color(0, 0, 0, 0));
            }
        }
        p->fillRect({width() - 1, 0, 1, height()}, Color(0, 0, 0, 0));
    }
}

}//namespace r64fx