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
        grabsMouseOnClick(false);
        ungrabsMouseOnRelease(false);

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
        grabsMouseOnClick(false);
        ungrabsMouseOnRelease(false);

        auto img = createCaptionImage(caption);
        if(!img)
            return;

        m_image = img;
        setSize({m_image->width(), m_image->height()});

        sub_menu->setParentItem(this);
    }


    virtual ~Widget_MenuItem()
    {
        delete m_image;
    }


    Action* action() const
    {
        return m_action;
    }


    Widget_Menu* subMenu() const
    {
        return m_sub_menu;
    }


    Widget_Menu* parentMenu() const
    {
        if(!parent())
            return nullptr;

        return dynamic_cast<Widget_Menu*>(parent());
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

    void activate()
    {
        auto parent_menu = parentMenu();
        if(parent_menu)
        {
            auto root_menu = parent_menu->rootMenu();

            if(m_action)
            {
                root_menu->closeAll();
                root_menu->update();
                m_action->exec();
            }
            else if(m_sub_menu && showSubMenu())
            {
                if(parent_menu->activeItem())
                {
                    auto active_menu_item = dynamic_cast<Widget_MenuItem*>(parent_menu->activeItem());
                    if(active_menu_item && active_menu_item->subMenu())
                    {
                        active_menu_item->subMenu()->closeAll();
                    }
                }
                parent_menu->setActiveItem(this);
            }
        }
    }

protected:
    virtual void updateEvent(UpdateEvent* event);

    virtual void mousePressEvent(MousePressEvent*);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();

private:
    bool showSubMenu();
};

}//namespace


Widget_Menu::Widget_Menu(Widget* parent)
: Widget(parent)
{
    grabsMouseOnClick(true);
    ungrabsMouseOnRelease(false);
    init_menu_font_if_needed();
}


void Widget_Menu::setParentItem(Widget* parent_item)
{
    m_parent_item = parent_item;
}


Widget* Widget_Menu::parentItem() const
{
    return m_parent_item;
}


void Widget_Menu::setActiveItem(Widget* active_item)
{
    m_active_item = active_item;
}


Widget* Widget_Menu::activeItem() const
{
    return m_active_item;
}


Widget_Menu* Widget_Menu::rootMenu()
{
    if(!m_parent_item)
        return this;

    auto menu_item = dynamic_cast<Widget_MenuItem*>(m_parent_item);
    if(!menu_item)
        return this;

    if(!menu_item->parent())
        return this;

    auto parent_menu = dynamic_cast<Widget_Menu*>(menu_item->parent());
    if(!parent_menu)
        return this;

    return parent_menu->rootMenu();
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
        Widget::grabMouse();
    }
}


bool Widget_MenuItem::showSubMenu()
{
    /*We work in screen coordinates here!*/

    auto parent_menu = parentMenu();
    if(!parent_menu)
        return false;

    auto parent_window = parent_menu->root()->window();
    if(!parent_window)
        return false;

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
    return true;
}


void Widget_Menu::closeAll()
{
    if(m_active_item)
    {
        auto active_menu_item = dynamic_cast<Widget_MenuItem*>(m_active_item);
        if(active_menu_item && active_menu_item->subMenu())
        {
            active_menu_item->subMenu()->closeAll();
        }
    }

    setActiveItem(nullptr);

    if(Widget::isWindow())
    {
        Widget::close();
    }
}


void Widget_MenuItem::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();

    static unsigned char grey       [4] = {127, 127, 127,  0};
    static unsigned char orange     [4] = {255, 127,  63,  0};
    static unsigned char blue       [4] = { 63, 127, 255,  0};
    static unsigned char black      [4] = {  0,   0,   0,  0};

    unsigned char* color = grey;

    auto parent_menu = parentMenu();

    if(m_flags & R64FX_WIDGET_IS_HOVERED)
    {
        color = orange;
    }
    else if(parent_menu->activeItem() == this)
    {
        color = blue;
    }

    p->fillRect({{0, 0}, size()}, color);
    if(m_image)
    {
        unsigned char* colors = (unsigned char*)&black;
        p->blendColors({0, 0}, &colors, m_image);
    }

    Widget::updateEvent(event);
}


void Widget_Menu::mousePressEvent(MousePressEvent* event)
{
    cout << "Widget_Menu: " << event->position() << "\n";
}


void Widget_MenuItem::mousePressEvent(MousePressEvent*)
{
    activate();
}


void Widget_Menu::mouseReleaseEvent(MouseReleaseEvent* event)
{

}


void Widget_Menu::mouseMoveEvent(MouseMoveEvent* event)
{

}


void Widget_MenuItem::mouseEnterEvent()
{
    auto parent_menu = parentMenu();
    if(parent_menu->activeItem())
    {
        activate();
    }
    update();
}


void Widget_MenuItem::mouseLeaveEvent()
{
    update();
}

}//namespace r64fx