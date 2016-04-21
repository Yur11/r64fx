#include "Widget_Menu.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "LayoutUtils.hpp"
#include "Font.hpp"
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
    Image*        m_image     = nullptr;
    Action*       m_action    = nullptr;
    Widget_Menu*  m_sub_menu  = nullptr;

    Image* createCaptionImage(const std::string &caption_text)
    {
        return text2image(caption_text, TextWrap::None, g_menu_font);
    }

public:
    Widget_MenuItem(Action* action, Widget_Menu* parent)
    : Widget(parent)
    , m_action(action)
    {
        auto img = createCaptionImage(action->caption());
        if(!img)
            return;

        m_image = img;
        setSize({m_image->width(), m_image->height()});
    }


    Widget_MenuItem(Widget_Menu* sub_menu, const std::string &caption, Widget_Menu* parent)
    : Widget(parent)
    , m_sub_menu(sub_menu)
    {
        auto img = createCaptionImage(caption);
        if(!img)
            return;

        m_image = img;
        setSize({m_image->width(), m_image->height()});
    }


    virtual ~Widget_MenuItem()
    {
        delete m_image;
    }


    void setSizeAndOffset(Size<int> size, Point<int> offset)
    {
        if(!m_image)
            return;

        auto new_image = new Image(size.width(), size.height(), m_image->componentCount());
        fill(new_image, (unsigned char)0);
        implant(new_image, offset, m_image);
        delete m_image;
        m_image = new_image;

        setSize({m_image->width(), m_image->height()});
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
        auto parent_menu = (Widget_Menu*) parent();
        if(m_action)
        {
            if(parent_menu && parent_menu->isWindow())
            {
                parent_menu->window()->ungrabMouse();
                parent_menu->close();
            }
            m_action->exec();
        }
        else if(m_sub_menu)
        {
            showSubMenu();
        }
    }


    virtual void mouseEnterEvent()
    {
        update();
    }


    virtual void mouseLeaveEvent()
    {
        update();
    }


private:
    void showSubMenu();
};

}//namespace


Widget_Menu::Widget_Menu(Widget* parent)
: Widget(parent)
{
    init_menu_font_if_needed();
}


void Widget_Menu::addAction(Action* action)
{
    if(action)
        new Widget_MenuItem(action, this);
}


void Widget_Menu::addSubMenu(Widget_Menu* menu, const std::string &caption)
{
    if(menu)
        new Widget_MenuItem(menu, caption, this);
}


void Widget_Menu::resizeAndReallign()
{
    /* Resize each menu item. */
    if(orientation() == Orientation::Vertical)
    {
        int max_width = 0;
        int total_height = 0;
        for(auto child : *this)
        {
            child->setPosition({0, total_height});
            if(child->width() > max_width)
            {
                max_width = child->width();
            }
            total_height += child->height();
        }

        for(auto child : *this)
        {
            auto menu_item = dynamic_cast<Widget_MenuItem*>(child);
            if(menu_item)
            {
                menu_item->setSizeAndOffset({max_width + 10, child->height() + 10}, {5, 5});
            }
        }
    }
    else
    {
        int total_width = 0;
        int max_height = 0;
        for(auto child : *this)
        {
            total_width += child->width();
            if(child->height() > max_height)
            {
                max_height = child->height();
            }
        }

        for(auto child : *this)
        {
            auto menu_item = dynamic_cast<Widget_MenuItem*>(child);
            if(menu_item)
            {
                menu_item->setSizeAndOffset({child->width() + 10, max_height + 10}, {5, 5});
            }
        }
    }


    /* Arrange menu. */
    Size<int> new_size = {0, 0};
    if(orientation() == Orientation::Vertical)
    {
        new_size = align_vertically(
            begin(), end(), {0, 0}, 0
        );
    }
    else
    {
        new_size = align_horizontally(
            begin(), end(), {0, 0}, 0
        );
    }
    setSize(new_size);
}


void Widget_Menu::showAt(Point<int> position, Widget* parent)
{
    auto parent_window = parent->root()->window();
    if(parent_window)
    {
        Size<int> screen_size = parent_window->getScreenSize();
        Point<int> menu_position = position + parent_window->position();

        if(menu_position.x() + width() > screen_size.width())
        {
            menu_position.setX(menu_position.x() - width());
        }

        if(menu_position.y() + height() > screen_size.height())
        {
            menu_position.setY(menu_position.y() - height());
        }

        show(Window::WmType::Menu, Window::Type::Image, parent_window);
        Widget::window()->setPosition(menu_position);
        Widget::window()->grabMouse();
    }
}


void Widget_MenuItem::showSubMenu()
{
    /*We work in screen coordinates here!*/

    auto parent_menu = dynamic_cast<Widget_Menu*>(parent());
    if(!parent_menu)
        return;

    auto parent_window = parent_menu->root()->window();
    if(!parent_window)
        return;

    Size<int> screen_size = parent_window->getScreenSize();

    Point<int> menu_item_position
        = toRootCoords(Point<int>(0, 0)) + parent_window->position();

    Point<int> sub_menu_position = {0, 0};

    if(parent_menu->orientation() == Orientation::Vertical)
    {
        if(menu_item_position.x() + parent_menu->width() + m_sub_menu->width() <= screen_size.width())
        {
            sub_menu_position.setX(
                menu_item_position.x() + parent_menu->width()
            );
        }
        else
        {
            sub_menu_position.setX(
                menu_item_position.x() - m_sub_menu->width()
            );
        }

        if(menu_item_position.y() + m_sub_menu->height() <= screen_size.height())
        {
            sub_menu_position.setY(
                menu_item_position.y()
            );
        }
        else
        {
            sub_menu_position.setY(
                menu_item_position.y() - m_sub_menu->height() + height()
            );
        }
    }
    else //Parent menu is horizontal.
    {
        if(menu_item_position.x() + m_sub_menu->width() <= screen_size.width())
        {
            sub_menu_position.setX(
                menu_item_position.x()
            );
        }
        else
        {
            sub_menu_position.setX(
                menu_item_position.x() - m_sub_menu->width() + width()
            );
        }

        if(menu_item_position.y() + parent_menu->height() + m_sub_menu->height() <= screen_size.height())
        {
            sub_menu_position.setY(
                menu_item_position.y() + parent_menu->height()
            );
        }
        else
        {
            sub_menu_position.setY(
                menu_item_position.y() - m_sub_menu->height()
            );
        }
    }

    m_sub_menu->show(Window::WmType::Menu, Window::Type::Image);
    m_sub_menu->window()->setPosition(sub_menu_position);
}

}//namespace r64fx