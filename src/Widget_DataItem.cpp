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


Widget_DataItem::~Widget_DataItem()
{
    if(m_image)
    {
        delete m_image;
    }
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
    setWidth(find_text_bbox(m_text, TextWrap::None, g_data_item_font).width() + g_data_item_font->height());
    setHeight(g_data_item_font->height());
}


Widget_DataItem* Widget_DataItem::parentDataItem()
{
    if(!parent())
        return nullptr;

    auto parent_data_item = dynamic_cast<Widget_DataItem*>(parent());
    if(!parent_data_item)
        return nullptr;

    return parent_data_item;
}


Widget_DataItem* Widget_DataItem::rootDataItem()
{
    auto parent_data_item = parentDataItem();
    if(!parent_data_item)
        return this;
    else
        return parent_data_item->rootDataItem();
}


void Widget_DataItem::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    if(!m_image)
    {
        m_image = new Image;
        text2image(m_text, TextWrap::None, g_data_item_font, m_image);
    }

    if(m_image)
    {
        int offset = g_data_item_font->height();

        unsigned char grey[4] = {175, 175, 175, 0};
        p->fillRect({0, 0, width(), height()}, grey);

        unsigned char red[4] = {127, 0, 0, 0};
        p->fillRect({2, 2, offset - 4, offset - 4}, red);

        unsigned char normal [4] = {0, 0, 0, 0};
        unsigned char hovered[4] = {255, 255, 255, 0};

        unsigned char* colors;
        if(Widget::isHovered())
            colors = hovered;
        else
            colors = normal;

        p->blendColors({offset, 0}, &colors, m_image);
    }

    Widget::paintEvent(event);
}


void Widget_DataItem::mousePressEvent(MousePressEvent* event)
{
    cout << "press: " << text() << " -> " << isVisible() << " -> " << this << "\n";
}


void Widget_DataItem::mouseEnterEvent()
{
    repaint();
}


void Widget_DataItem::mouseLeaveEvent()
{
    repaint();
}

}//namespace