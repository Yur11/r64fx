#ifndef R64FX_WIDGET_EVENTS_HPP
#define R64FX_WIDGET_EVENTS_HPP

namespace r64fx{
    
class Painter;
class PainterTextureManager;
class WidgetImpl;

class WidgetPaintEvent{
    friend class WidgetImpl;

    WidgetImpl* m_impl = nullptr;

    WidgetPaintEvent(WidgetImpl* impl)
    : m_impl(impl)
    {}

    WidgetPaintEvent(const WidgetPaintEvent&) {}

public:
    WidgetImpl* impl() const;

    Painter* painter() const;
};


class WidgetResizeEvent{
    Size<int> m_old_size;
    Size<int> m_new_size;

    WidgetResizeEvent(const WidgetResizeEvent&) {}

public:
    WidgetResizeEvent(Size<int> old_size, Size<int> new_size)
    : m_old_size(old_size)
    , m_new_size(new_size)
    {}

    inline Size<int> size() const { return m_new_size; }

    inline Size<int> oldSize() const { return m_old_size; }

    inline int width()  const { return m_new_size.width(); }

    inline int height() const { return m_new_size.height(); }

    inline int oldWidth() const { return m_old_size.width(); }

    inline int oldHeight() const { return m_old_size.height(); }

    inline bool widthChanged() const { return width() != oldWidth(); }

    inline bool heightChanged() const { return height() != oldHeight(); }
};


class WidgetClipEvent{
    Rect<int> m_rect;

public:
    WidgetClipEvent(Rect<int> rect) : m_rect(rect) {}

    inline Rect<int> rect() const { return m_rect; }
};


class MouseEvent{
    Point<int>  m_position;
    Point<int>  m_delta;
    MouseButton m_button;

public:
    bool ignore_grabs = false;

    explicit MouseEvent(Point<int> position, Point<int> delta, MouseButton button)
    : m_position(position)
    , m_delta(delta)
    , m_button(button)
    {

    }

    inline void setPosition(Point<int> position) { m_position = position; }

    inline Point<int> position() const { return m_position; }

    inline int x() const { return m_position.x(); }
    inline int y() const { return m_position.y(); }

    inline Point<int> delta() const { return m_delta; }

    inline int dx() const { return m_delta.x(); }
    inline int dy() const { return m_delta.y(); }

    inline MouseButton button() const { return m_button; }
};


class WidgetWindowAvailabilityEvent{
    Window* m_window = nullptr;
    PainterTextureManager* m_ptm = nullptr;
    
public:
    WidgetWindowAvailabilityEvent(Window* window, PainterTextureManager* ptm) 
    : m_window(window)
    , m_ptm(ptm)
    {
        
    }
    
    inline Window* window() const
    {
        return m_window;
    }
    
    inline PainterTextureManager* textureManager() const
    {
        return m_ptm;
    }
};


class WidgetAddedToWindowEvent : public WidgetWindowAvailabilityEvent{
public:
    using WidgetWindowAvailabilityEvent::WidgetWindowAvailabilityEvent;
};


class WidgetRemovedFromWindowEvent : public WidgetWindowAvailabilityEvent{
public:
    using WidgetWindowAvailabilityEvent::WidgetWindowAvailabilityEvent;
};


class MousePressEvent : public MouseEvent{
public:
    using MouseEvent::MouseEvent;
};


class MouseReleaseEvent : public MouseEvent{
public:
    using MouseEvent::MouseEvent;
};


class MouseMoveEvent : public MouseEvent{
public:
    using MouseEvent::MouseEvent;
};


class KeyEvent{
    int m_key;

public:
    explicit KeyEvent(int key)
    : m_key(key)
    {}

    inline int key() const { return m_key; }
};


class KeyPressEvent : public KeyEvent{
public:
    explicit KeyPressEvent(int key) : KeyEvent(key) {}
};


class KeyReleaseEvent : public KeyEvent{
public:
    explicit KeyReleaseEvent(int key) : KeyEvent(key) {}
};


class TextInputEvent : public KeyEvent{
    std::string m_text;

public:
    explicit TextInputEvent(const std::string &text, int key)
    : KeyEvent(key)
    , m_text(text)
    {
    }

    inline std::string text() const { return m_text; }
};

}//namespace r64fx

#endif//R64FX_WIDGET_EVENTS_HPP
