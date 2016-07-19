#ifndef R64FX_WIDGET_MENU_HPP
#define R64FX_WIDGET_MENU_HPP

#include "Widget.hpp"
#include "Action.hpp"

namespace r64fx{

class Widget_Menu : public Widget{
    Widget* m_parent_item = nullptr;
    Widget* m_active_item = nullptr;

public:
    Widget_Menu(Widget* parent = nullptr);


    void setParentItem(Widget* parent_item);

    Widget* parentItem() const;

    void setActiveItem(Widget* active_item);

    Widget* activeItem() const;

    Widget_Menu* rootMenu();


    void addAction(Action* action);

    void addSubMenu(Widget_Menu* menu, const std::string &caption);

    void resizeAndRealign();

    
    void showAt(Point<int> position, Widget* parent);

    void closeAll();

public:
    virtual void mousePressEvent(MousePressEvent* event);

    virtual void clickedElsewhereEvent();
};

}//namespace r64fx

#endif//R64FX_WIDGET_MENU_HPP