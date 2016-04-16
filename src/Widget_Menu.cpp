#include "Widget_Menu.hpp"
#include "Widget_Control.hpp"
#include "Font.hpp"
#include "LayoutUtils.hpp"

namespace r64fx{

namespace{
    Font* g_menu_font = nullptr;

    void init_menu_font_if_needed()
    {
        if(g_menu_font)
            return;

        g_menu_font = new Font("", 14, 72);
    }
}

Widget_Menu::Widget_Menu(Widget* parent)
: Widget(parent)
{
    init_menu_font_if_needed();
}


void Widget_Menu::addItem(const std::string &caption)
{
    auto menu_item_animation = new ControlAnimation_MenuItem(caption, g_menu_font);
    new Widget_Control(menu_item_animation, this);
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