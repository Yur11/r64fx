#ifndef R64FX_WIDGET_H
#define R64FX_WIDGET_H

#include "LinkedList.hpp"
#include "Rect.hpp"

namespace r64fx{

class Window;
class MouseEvent;
class KeyEvent;
class ResizeEvent;

class Widget : public LinkedItem<Widget>{
    friend class Program; //To call event methods.

    /* A widget may either have another widget as a parent
     * or it can be shown in a window, but not both at the same time.
     */
    union {
        Widget* widget = nullptr;
        Window* window;
    } m_parent;

    /* Storage for widget flags.
     * These are bit-packed and used internally.
     */
    unsigned long m_flags = 0;
    
    /* Widgets geometry. */
    Rect<int> m_rect = { 0, 0, 0, 0  };

    /* A linked list of widgets children. */
    LinkedList<Widget> m_children;
    
public:
    Widget(Widget* parent = nullptr);
    
    virtual ~Widget();
    
    void setParent(Widget* parent);
    
    Widget* parent() const;
    
    void add(Widget* child);
    
    void resize(Size<int> size);

    void resize(int w, int h);
    
    Size<int> size() const;

    int width() const;

    int height() const;
    
    /** @brief Show this widget in a window. */
    void show();

    /** @brief Hide this widget if it is shown in a window.*/
    void hide();

    bool isWindow() const;

    
protected:
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    virtual void keyReleaseEvent(KeyEvent* event);

    virtual void resizeEvent(ResizeEvent* event);
};
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H