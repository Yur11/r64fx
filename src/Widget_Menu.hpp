#ifndef R64FX_WIDGET_MENU_HPP
#define R64FX_WIDGET_MENU_HPP

#include "Widget.hpp"
#include "Padding.hpp"

namespace r64fx{

class Widget_Menu : public Widget, public Padding<int>{
public:
    Widget_Menu(Widget* parent = nullptr);

    void addItem(const std::string &caption);

    void resizeAndReallign();
};

}//namespace r64fx

#endif//R64FX_WIDGET_MENU_HPP