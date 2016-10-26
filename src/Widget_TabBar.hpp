#ifndef R64FX_WIDGET_TAB_BAR_HPP
#define R64FX_WIDGET_TAB_BAR_HPP

#include "Widget.hpp"
#include "IconNames.hpp"

namespace r64fx{
    
class TabHandle;
    
class Widget_TabBar : public Widget{
    friend class TabHandle;
    TabHandle* m_current_tab = nullptr;
    
    void (*m_tab_selected)(TabHandle* handle, void* payload, void* arg) = nullptr;
    void* m_tab_selected_arg = nullptr;

public:
    Widget_TabBar(Widget* parent = nullptr);
    
    virtual ~Widget_TabBar();
    
    virtual TabHandle* addTab(void* tab_payload, const std::string &caption = "", IconName icon_name = IconName::None);
    
    void resizeAndRealign();

    void onTabSelected(void (tab_selected)(TabHandle* handle, void* payload, void* arg), void* arg = nullptr);
    
    TabHandle* currentTab() const;
    
    void selectTab(TabHandle* tab_handle);
    
    int currentTabX() const;
    
    int currentTabWidth() const;
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event);
};
    
}//namespace r64fx

#endif//R64FX_WIDGET_TAB_BAR_HPP
