#include "Widget_DataItem.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

Font* g_data_item_font = nullptr;

void init_data_item_font_if_needed()
{
    if(g_data_item_font)
        return;

    g_data_item_font = new Font("", 14, 72);
}

}//namespace


Widget_DataItem::Widget_DataItem(const std::string &text, Widget* parent)
: Widget(parent)
, m_text(text)
{
    init_data_item_font_if_needed();
}


Widget_DataItem::Widget_DataItem(Widget* parent)
: Widget(parent)
{
    init_data_item_font_if_needed();
}


void Widget_DataItem::setText(const std::string &text)
{
    m_text = text;
}


std::string Widget_DataItem::text() const
{
    return m_text;
}


void Widget_DataItem::resizeAndReallign()
{
    setWidth(find_text_bbox(m_text, TextWrap::None, g_data_item_font).width());
    setHeight(g_data_item_font->height());
}


void Widget_DataItem::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();

    Image img;
    if(text2image(m_text, TextWrap::None, g_data_item_font, &img))
    {
        unsigned char grey[4] = {175, 175, 175, 0};
        p->fillRect({0, 0, img.width(), img.height()}, grey);


        unsigned char normal [4] = {0, 0, 0, 0};
        unsigned char hovered[4] = {255, 255, 255, 0};

        unsigned char* colors;
        if(Widget::isHovered())
            colors = hovered;
        else
            colors = normal;

        p->blendColors({0, 0}, &colors, &img);
    }

    Widget::updateEvent(event);
}


void Widget_DataItem::mouseEnterEvent()
{
    update();
}


void Widget_DataItem::mouseLeaveEvent()
{
    update();
}

}//namespace