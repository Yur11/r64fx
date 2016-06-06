#include "Widget_DataItem.hpp"
#include "Widget_ItemTree.hpp"
#include "Widget_Label.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "ImageUtils.hpp"
#include "ClipboardEvent.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    Font*   g_data_item_font = nullptr;
    Image*  g_button_img_down   = nullptr;
    Image*  g_button_img_right  = nullptr;
    int     g_button_img_offset = 0;

    void init()
    {
        g_data_item_font  = new Font("", 14, 72);
        g_button_img_down   = new Image;
        g_button_img_right  = new Image;

        int img_size = g_data_item_font->height();
        g_button_img_offset = 0;
        if(img_size > 8)
        {
            img_size -= 8;
            g_button_img_offset = 4;
        }
        draw_triangles(img_size, nullptr, g_button_img_down, nullptr, g_button_img_right);
    }

    void cleanup()
    {
        if(g_button_img_down)
            delete g_button_img_down;

        if(g_button_img_right)
            delete g_button_img_right;

       g_button_img_down = g_button_img_right = nullptr;

       delete g_data_item_font;
       g_data_item_font = nullptr;
    }

    int g_data_item_count = 0;

    void init_if_needed()
    {
        if(g_data_item_count == 0)
        {
            init();
        }
        g_data_item_count++;
    }
}//namespace


Widget_DataItem::Widget_DataItem(const std::string &text, Widget* parent)
: Widget(parent)
, m_text(text)
{
    init_if_needed();
}


Widget_DataItem::Widget_DataItem(Widget* parent)
: Widget(parent)
{
    init_if_needed();
}


Widget_DataItem::~Widget_DataItem()
{
    if(m_image)
    {
        delete m_image;
    }

    g_data_item_count--;
    if(g_data_item_count == 0)
    {
        cleanup();
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


int Widget_DataItem::lineHeight()
{
    return g_data_item_font->height();
}


void Widget_DataItem::resizeAndReallign(int min_width)
{
    int width = find_text_bbox(m_text, TextWrap::None, g_data_item_font).width() + g_data_item_font->height();
    if(width < min_width)
        width = min_width;

    setWidth(width);
    setHeight(lineHeight());
}


int Widget_DataItem::enumerate(int num)
{
    if(num & 1)
        m_flags &= ~R64FX_WIDGET_IS_EVEN;
    else
        m_flags |= R64FX_WIDGET_IS_EVEN;
    return num + 1;
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


void Widget_DataItem::setTreeDepth(int depth)
{
    m_tree_depth = depth;
}


int Widget_DataItem::treeDepth() const
{
    return m_tree_depth;
}


int Widget_DataItem::indentWidth() const
{
    return treeDepth() * g_data_item_font->height();
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
        auto old_clip_rect = p->clipRect();
        p->setClipRect(Rect<int>(toRootCoords(m_visible_rect.position()), m_visible_rect.size()));

        int offset = indentWidth();

        unsigned char odd_bg[4]  = {200, 200, 200, 0};
        unsigned char even_bg[4] = {175, 175, 175, 0};
        p->fillRect({0, 0, width() + offset, height()}, ((m_flags & R64FX_WIDGET_IS_EVEN) ? even_bg : odd_bg));

        unsigned char normal [4] = {0, 0, 0, 0};
        unsigned char hovered[4] = {255, 255, 255, 0};

        unsigned char* colors;
        if(Widget::isHovered())
            colors = hovered;
        else
            colors = normal;


        auto item_tree = dynamic_cast<Widget_ItemTree*>(this);
        if(item_tree)
        {
            Image* button_img = nullptr;
            if(item_tree->isCollapsed())
                button_img = g_button_img_right;
            else
                button_img = g_button_img_down;
            p->blendColors({offset + g_button_img_offset, g_button_img_offset}, &colors, button_img);
            p->blendColors({offset + m_image->height(), 0}, &colors, m_image);
        }
        else
        {
            p->blendColors({offset, 0}, &colors, m_image);
        }

        p->setClipRect(old_clip_rect);
    }

    Widget::paintEvent(event);
}


void Widget_DataItem::clipEvent(ClipEvent* event)
{
    m_visible_rect = event->rect();
}


void Widget_DataItem::mousePressEvent(MousePressEvent* event)
{
    grabMouse();
    Widget::mousePressEvent(event);
}


void Widget_DataItem::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(isMouseGrabber())
    {
        ungrabMouse();
    }
    Widget::mouseReleaseEvent(event);
}


void Widget_DataItem::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        int distance = event->dx() * event->dx() + event->dy() * event->dy();
        if(distance > 1 && (!text().empty()))
        {
            auto label = new Widget_Label(text());
            Point<int> anchor(event->x() - indentWidth(), event->y());
            if(anchor.x() < 0 )
                anchor.setX(0);
            else if(anchor.x() > label->width())
                anchor.setX(label->width() - 1);
            startDrag(label, anchor);
        }
    }
}


void Widget_DataItem::mouseEnterEvent()
{
    repaint();
}


void Widget_DataItem::mouseLeaveEvent()
{
    repaint();
}


void Widget_DataItem::dndFinishedEvent(DndFinishedEvent* event)
{
    auto dnd_object = event->dndObject();
    if(dnd_object)
    {
        delete dnd_object;
    }
}

}//namespace