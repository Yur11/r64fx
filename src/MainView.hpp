#ifndef R64FX_MAIN_VIEW_HPP
#define R64FX_MAIN_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class MainViewPrivate;
    
class MainView : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    MainView(Widget* parent = nullptr);
    
    virtual ~MainView();
    
protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);
    
    virtual void mouseMoveEvent(MouseMoveEvent* event);
    
    virtual void closeEvent();
};
    
}//namespace r64fx

#endif//R64FX_MAIN_VIEW_HPP
