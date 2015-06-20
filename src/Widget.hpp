#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include "LinkedList.hpp"
#include "Rect.hpp"

namespace r64fx{

class Widget;    
class Window;

class Widget : public LinkedItem<Widget>{    
    Widget* m_parent = nullptr;
    LinkedList<Widget> m_children;
    
    Rect<int> m_rect = { 0, 0, 0, 0  };
    
    Window* m_host_window = nullptr;
    
    Widget(const Widget &other) {}
    
public:
    Widget(Widget* parent = nullptr);
    
    virtual ~Widget();
    
    void setParent(Widget* parent);
    
    inline Widget* parent() const { return m_parent; }
    
    inline void removeFromParent() { setParent(nullptr); }
    
    void add(Widget* child);
    
    /** @brief Show this widget in a window. */
    Window* show();
    
    void hide();
    
    inline Window* hostWindow() const { return m_host_window; }
    
    inline bool isWindow() const { return hostWindow() != nullptr; }
    
    void resize(int w, int h);
    
    inline int width()  const { return m_rect.width(); }

    inline int height() const { return m_rect.height(); }
    
    inline Size<int> size() const { return m_rect.size(); }
};  
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H