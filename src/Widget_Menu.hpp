#ifndef R64FX_WIDGET_MENU_HPP
#define R64FX_WIDGET_MENU_HPP

#include "Widget.hpp"
#include "Action.hpp"

namespace r64fx{

class Widget_Menu : public Widget{
    Widget_Menu* m_parent_menu = nullptr;

public:
    Widget_Menu(Widget* parent = nullptr);

    void addAction(Action* action);

    void addSubMenu(Widget_Menu* menu, const std::string &caption);

    void resizeAndReallign();

    void showAt(Point<int> position, Widget* parent);
};

}//namespace r64fx

#endif//R64FX_WIDGET_MENU_HPP