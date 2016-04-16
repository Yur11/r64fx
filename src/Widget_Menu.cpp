#include "Widget_Menu.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "LayoutUtils.hpp"
#include "TextPainter.hpp"
#include <iostream>

using namespace std;

namespace r64fx{

namespace{

Font* g_menu_font = nullptr;

void init_menu_font_if_needed()
{
    if(g_menu_font)
        return;

    g_menu_font = new Font("", 14, 72);
}


class Widget_MenuItem : public Widget{
    Image* m_image = nullptr;

public:
    Widget_MenuItem(const std::string &caption, Widget_Menu* parent)
    : Widget(parent)
    {
        auto img = text2image(caption, TextWrap::None, g_menu_font);
        if(!img)
            return;

        m_image = img;
        setSize({img->width(), img->height()});
    }


    virtual ~Widget_MenuItem()
    {
        delete m_image;
    }


protected:
    virtual void updateEvent(UpdateEvent* event)
    {
        auto p = event->painter();

        static unsigned char normal_bg  [4] = {127, 127, 127,  0};
        static unsigned char hovered_bg [4] = {255, 127,  63,  0};
        static unsigned char text_color [4] = {  0,   0,   0,  0};

        unsigned char* color = normal_bg;
        if(m_flags & R64FX_WIDGET_IS_HOVERED)
        {
            color = hovered_bg;
        }

        p->fillRect({{0, 0}, size()}, color);
        if(m_image)
        {
            unsigned char* colors = (unsigned char*)&text_color;
            p->blendColors({0, 0}, &colors, m_image);
        }

        Widget::updateEvent(event);
    }


    virtual void mousePressEvent(MousePressEvent*)
    {

    }


    virtual void mouseEnterEvent()
    {
        update();
    }


    virtual void mouseLeaveEvent()
    {
        update();
    }
};

}//namespace


Widget_Menu::Widget_Menu(Widget* parent)
: Widget(parent)
{
    init_menu_font_if_needed();
}


void Widget_Menu::addItem(const std::string &caption)
{
    auto item = new Widget_MenuItem(caption, this);
}


void Widget_Menu::resizeAndReallign()
{
    Size<int> new_size = {0, 0};
    if(orientation() == Orientation::Vertical)
    {
        new_size = align_vertically(
            begin(), end(), {paddingLeft(), paddingTop()}, 0
        );
    }
    else
    {
        new_size = align_horizontally(
            begin(), end(), {paddingLeft(), paddingTop()}, 0
        );
    }
    new_size += Size<int>(paddingRight(), paddingBottom());
    setSize(new_size);
}

}//namespace r64fx