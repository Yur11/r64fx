#ifndef R64FX_WIDGET_TAB_BAR_HPP
#define R64FX_WIDGET_TAB_BAR_HPP

#include "Widget.hpp"

namespace r64fx{
    
class TabHandle;
    
class Widget_TabBar : public Widget{
public:
    Widget_TabBar(Widget* parent = nullptr);
    
    virtual ~Widget_TabBar();
    
    virtual TabHandle* addTab(void* tab_payload, const std::string &caption = "");
    
    void resizeAndRealign();
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event);
    
    virtual void tabSelected(TabHandle* tab, void* tab_payload);
};
    
}//namespace r64fx

#endif//R64FX_WIDGET_TAB_BAR_HPP
