#ifndef R64FX_MAIN_VIEW_HPP
#define R64FX_MAIN_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class MainViewPrivate;
class MainViewEventIface;

class MainView : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    MainView(MainViewEventIface* event_iface, Widget* parent = nullptr);
    
    virtual ~MainView();

    void addMainPartOption(void* option, const std::string &name);
    
    void setMainPartWidget(Widget* widget);
    
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


class MainViewEventIface{
public:
    virtual void mainPartOptionSelected(void* option) = 0;
};
    
}//namespace r64fx

#endif//R64FX_MAIN_VIEW_HPP
