#include "Widget_DataItem.hpp"
#include "Widget_ItemBrowser.hpp"
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


Widget_DataItem::Widget_DataItem(const std::string &caption, Widget* parent)
: Widget(parent)
, m_caption(caption)
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


void Widget_DataItem::setCaption(const std::string &caption)
{
    m_caption = caption;
}


std::string Widget_DataItem::caption() const
{
    return m_caption;
}


void Widget_DataItem::setKind(Widget_DataItem::Kind kind)
{
    m_flags &= ~(R64FX_WIDGET_DATA_ITEM_IS_COMPOUND | R64FX_WIDGET_DATA_ITEM_IS_TREE);

    if(kind != Widget_DataItem::Kind::Plain)
    {
        m_flags |= R64FX_WIDGET_DATA_ITEM_IS_COMPOUND;
        if(kind == Widget_DataItem::Kind::Tree)
        {
            m_flags |= R64FX_WIDGET_DATA_ITEM_IS_TREE;
        }
    }
}


Widget_DataItem::Kind Widget_DataItem::kind()
{
    if(m_flags & R64FX_WIDGET_DATA_ITEM_IS_COMPOUND)
    {
        if(m_flags & R64FX_WIDGET_DATA_ITEM_IS_TREE)
        {
            return Widget_DataItem::Kind::Tree;
        }
        else
        {
            return Widget_DataItem::Kind::List;
        }
    }
    else
    {
        return Widget_DataItem::Kind::Plain;
    }
}


int Widget_DataItem::lineHeight()
{
    return g_data_item_font->height();
}


void Widget_DataItem::addItem(Widget_DataItem* item)
{
    Widget::add(item);
}


void Widget_DataItem::addItem(const std::string &caption)
{
    Widget::add(new Widget_DataItem(caption));
}


void Widget_DataItem::resizeAndReallign(int min_width)
{
    setSize({0, 0});

    switch(kind())
    {
        case Widget_DataItem::Kind::Plain:
        {
            resizeAndReallignPlain(min_width);
            break;
        }

        case Widget_DataItem::Kind::List:
        {
            resizeAndReallignList(min_width);
            break;
        }

        case Widget_DataItem::Kind::Tree:
        {
            resizeAndReallignTree(min_width);
            break;
        }

        default:
            break;
    }
}


void Widget_DataItem::resizeAndReallignPlain(int min_width)
{
    int width = find_text_bbox(m_caption, TextWrap::None, g_data_item_font).width() + g_data_item_font->height();
    if(width < min_width)
        width = min_width;

    setWidth(width);
    setHeight(lineHeight());
}


void Widget_DataItem::resizeAndReallignList(int min_width)
{
    int max_child_width  = width();
    int total_height     = height();

    for(auto child : *this)
    {
        auto data_item = dynamic_cast<Widget_DataItem*>(child);
        if(data_item)
        {
            data_item->resizeAndReallign(min_width);
        }

        if(child->width() > max_child_width)
            max_child_width = child->width();

        total_height += child->height();
    }

    int running_y = height();
    for(auto child : *this)
    {
        child->setWidth(max_child_width);
        child->setX(0);
        child->setY(running_y);
        running_y += child->height();
    }

    setSize({max_child_width, total_height});
}


void Widget_DataItem::resizeAndReallignTree(int min_width)
{
    resizeAndReallignPlain(min_width);

    if(isExpanded())
    {
        resizeAndReallignList(min_width);
    }
}


int Widget_DataItem::enumerate(int num)
{
    switch(kind())
    {
        case Widget_DataItem::Kind::Plain:
        {
            return enumeratePlain(num);
        }

        case Widget_DataItem::Kind::List:
        {
            return enumerateList(num);
        }

        case Widget_DataItem::Kind::Tree:
        {
            return enumerateTree(num);
        }

        default:
        {
            return num;
        }
    }
}


int Widget_DataItem::enumeratePlain(int num)
{
    if(num & 1)
        m_flags &= ~R64FX_WIDGET_IS_EVEN;
    else
        m_flags |= R64FX_WIDGET_IS_EVEN;
    return num + 1;
}


int Widget_DataItem::enumerateList(int num)
{
    for(auto child : *this)
    {
        auto data_item = dynamic_cast<Widget_DataItem*>(child);
        if(data_item)
        {
            num = data_item->enumerate(num);
        }
    }
    return num;
}


int Widget_DataItem::enumerateTree(int num)
{
    num = enumeratePlain(num);
    if(!isExpanded())
        return num;

    return enumerateList(num);
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


Widget_ItemBrowser* Widget_DataItem::parentBrowser()
{
    auto root_item = rootDataItem();
    auto root_item_parent = root_item->parent();
    if(!root_item_parent)
        return nullptr;

    auto root_item_parent_parent = root_item_parent->parent();
    if(!root_item_parent_parent)
        return nullptr;

    auto item_browser = dynamic_cast<Widget_ItemBrowser*>(root_item_parent_parent);
    if(!item_browser)
        return nullptr;

    return item_browser;
}


int Widget_DataItem::indent() const
{
    auto parent_widget = parent();
    if(!parent_widget)
        return 0;

    auto parent_item = dynamic_cast<Widget_DataItem*>(parent_widget);
    if(!parent_item || parent_item->kind() != Widget_DataItem::Kind::Tree)
        return 0;

    return parent_item->indent() + 1;
}


int Widget_DataItem::indentWidth() const
{
    return indent() * g_data_item_font->height();
}


void Widget_DataItem::setSelected(bool yes)
{
    auto item_browser = parentBrowser();
    if(item_browser)
    {
        auto selected_item = item_browser->m_selected_item;
        if(selected_item)
        {
            selected_item->m_flags &= ~R64FX_WIDGET_IS_SELECTED;
            selected_item->repaint();
        }

        if(yes)
            item_browser->setSelectedItem(this);
        else
            item_browser->setSelectedItem(nullptr);
    }

    if(yes)
        m_flags |= R64FX_WIDGET_IS_SELECTED;
    else
        m_flags &= ~R64FX_WIDGET_IS_SELECTED;
    repaint();
}


bool Widget_DataItem::isSelected() const
{
    return m_flags & R64FX_WIDGET_IS_SELECTED;
}


void Widget_DataItem::collapse()
{
    if(kind() != Widget_DataItem::Kind::Tree)
        return;

    m_flags &= ~R64FX_WIDGET_TREE_IS_EXPANDED;
    auto root_item = rootDataItem();
    auto root_item_parent = root_item->parent();
    if(root_item_parent)
    {
        root_item->resizeAndReallign(root_item_parent->width());
        root_item->enumerate(0);
        root_item_parent->clip();
        root_item_parent->repaint();
    }
}


void Widget_DataItem::expand()
{
    if(kind() != Widget_DataItem::Kind::Tree)
        return;

    m_flags |= R64FX_WIDGET_TREE_IS_EXPANDED;
    auto root_item = rootDataItem();
    auto root_item_parent = root_item->parent();
    if(root_item_parent)
    {
        root_item->resizeAndReallign(root_item_parent->width());
        root_item->enumerate(0);
        root_item_parent->clip();
        root_item_parent->repaint();
    }
}


bool Widget_DataItem::isExpanded()
{
    return m_flags & R64FX_WIDGET_TREE_IS_EXPANDED;
}


void Widget_DataItem::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    if(kind() == Widget_DataItem::Kind::List)
    {
        unsigned char bg[4] = {200, 200, 200, 0};
        p->fillRect({{0, 0}, size()}, bg);
    }
    else
    {
        if(!m_image)
        {
            m_image = new Image;
            text2image(m_caption, TextWrap::None, g_data_item_font, m_image);
        }

        if(m_image)
        {
            auto old_clip_rect = p->clipRect();
            p->setClipRect(Rect<int>(toRootCoords(m_visible_rect.position()), m_visible_rect.size()));

            int offset = indentWidth();

            unsigned char odd_bg[4]  = {200, 200, 200, 0};
            unsigned char even_bg[4] = {190, 190, 190, 0};
            p->fillRect({0, 0, width() + offset, height()}, ((m_flags & R64FX_WIDGET_IS_EVEN) ? even_bg : odd_bg));

            unsigned char normal [4]  = {0, 0, 0, 0};
            unsigned char hovered[4]  = {0, 127, 0, 0};
            unsigned char selected[4] = {127, 0, 0, 255};

            unsigned char* colors;
            if(isSelected())
                colors = selected;
            else if(isHovered())
                colors = hovered;
            else
                colors = normal;

            if(kind() == Widget_DataItem::Kind::Tree)
            {
                Image* button_img = nullptr;
                if(isExpanded())
                    button_img = g_button_img_down;
                else
                    button_img = g_button_img_right;
                p->blendColors({offset + g_button_img_offset, g_button_img_offset}, &colors, button_img);
                p->blendColors({offset + m_image->height(), 0}, &colors, m_image);
            }
            else
            {
                p->blendColors({offset, 0}, &colors, m_image);
            }

            p->setClipRect(old_clip_rect);
        }
    }

    Widget::paintEvent(event);
}


void Widget_DataItem::clipEvent(ClipEvent* event)
{
    m_visible_rect = event->rect();
}


void Widget_DataItem::mousePressEvent(MousePressEvent* event)
{
    switch(kind())
    {
        case Widget_DataItem::Kind::Plain:
        {
            mousePressEventPlain(event);
            break;
        }

        case Widget_DataItem::Kind::Tree:
        {
            mousePressEventTree(event);
            break;
        }

        default:
        {
            Widget::mousePressEvent(event);
            break;
        }
    }
}


void Widget_DataItem::showContextMenu(Point<int> position)
{

}


void Widget_DataItem::mousePressEventPlain(MousePressEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        setSelected(true);
        setFocus();
    }
    else if(event->button() & MouseButton::Right())
    {
        showContextMenu(event->position());
    }
    grabMouse();
    Widget::mousePressEvent(event);
}


void Widget_DataItem::mousePressEventTree(MousePressEvent* event)
{
    if(event->y() <= lineHeight())
    {
        if((event->button() & MouseButton::Left()) && event->x() < (lineHeight() * (indent() + 1)))
        {
            if(isExpanded())
            {
                collapse();
            }
            else
            {
                expand();
            }
        }
        else
        {
            mousePressEventPlain(event);
        }
    }
    else
    {
        Widget::mousePressEvent(event);
    }

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
        if(distance > 1 && (!caption().empty()))
        {
            auto label = new Widget_Label(caption());
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