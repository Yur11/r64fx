#ifndef R64FX_WIDGET_CHECK_BOX_HPP
#define R64FX_WIDGET_CHECK_BOX_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_CheckBox : public Widget{
    void (*m_on_check)(bool checked, Widget_CheckBox*, void*) = nullptr;
    void* m_on_check_data = nullptr;

public:
    Widget_CheckBox(Widget* parent = nullptr);

    void check(bool notify = false);

    void uncheck(bool notify = false);

    bool isChecked() const;

    void onCheck(void (*on_check)(bool checked, Widget_CheckBox*, void*), void* on_check_data = nullptr);

private:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event);

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event);
    
    virtual void mousePressEvent(MousePressEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CHECK_BOX_HPP
