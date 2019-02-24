#ifndef R64FX_VIEW_PROGRAM_HPP
#define R64FX_VIEW_PROGRAM_HPP

#include "Widget_Menu.hpp"

namespace r64fx{

class View_ProgramPrivate;
class View_ProgramEventIface;

class View_Program : public Widget{
    View_ProgramPrivate* m = nullptr;

public:
    View_Program(View_ProgramEventIface* event_iface, Widget* parent = nullptr);

    virtual ~View_Program();

    void addMainPartOption(void* option, const std::string &name);

    void setMainPartWidget(Widget* widget);

    void addMenu(Widget_Menu* menu, const char* name);

    void resizeAndRealign();

protected:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void resizeEvent(WidgetResizeEvent* event);

    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event);

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void closeEvent();
};


class View_ProgramEventIface{
public:
    virtual void mainPartOptionSelected(void* option) = 0;
};

}//namespace r64fx

#endif//R64FX_VIEW_PROGRAM_HPP
