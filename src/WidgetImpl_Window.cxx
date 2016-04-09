/* To be included in Widget.cpp */

namespace r64fx{

/* Maximum number of individual rectangles
    * that can be repainted after reconf. cycle. */
constexpr int max_rects = 16;

/* Collection of data attached to the window.
    * We should be able to cast back and forth
    * between WindowWidgetData and Window::UpdateEvent. */
struct WindowWidgetData : Widget::UpdateEvent{

    /* Root widget shown in the window that
        * this context is attached to. */
    Widget*  widget = nullptr;

    /* Painter serving the window. */
    Painter* painter = nullptr;

    /* Current visible rect. passed to widget reconf. method. */
    Rect<int> visible_rect;

    /* List of rectangles to be repainted after reconf. cycle. */
    Rect<int> rects[max_rects];

    /* Number of rectangles that must be repainted. */
    int num_rects = 0;

    /* Used in reconf. logic. */
    bool got_rect = false;

    /* Total offset for every nested scrollable view. */
    Point<int> view_offset = {0, 0};
};


class WindowEvents_Widget : public WindowEvents{

    virtual void resizeEvent(Window* window, int width, int height)
        {
        auto d = (WindowWidgetData*) window->data();
        d->widget->setSize({width, height});
        d->widget->update();
    }


    virtual void mousePressEvent(Window* window, int x, int y, unsigned int button)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->initMousePressEvent(Point<int>(x, y), MouseButton(button));
    }


    virtual void mouseReleaseEvent(Window* window, int x, int y, unsigned int button)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->initMouseReleaseEvent(Point<int>(x, y), MouseButton(button));
    }


    virtual void mouseMoveEvent(Window* window, int x, int y)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->initMouseMoveEvent(Point<int>(x, y));
    }


    virtual void keyPressEvent(Window* window, unsigned int key)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->initKeyPressEvent(key);
    }


    virtual void keyReleaseEvent(Window* window, unsigned int key)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->initKeyReleaseEvent(key);
    }


    virtual void textInputEvent(Window* window, const std::string &text, unsigned int key)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->initTextInputEvent(text, key);
    }


    virtual void clipboardDataRecieveEvent(
        Window* window, ClipboardDataType type, void* data, int size, ClipboardMode mode
    )
    {
        auto widget = requestor(mode);
        if(widget)
        {
            ClipboardDataRecieveEvent event(mode, type, data, size);
            widget->clipboardDataRecieveEvent(&event);
        }
    }


    virtual void clipboardDataTransmitEvent(
        Window* window, ClipboardDataType type, void** data, int* size, ClipboardMode mode
    )
    {
        auto widget = anouncer(mode);
        if(widget)
        {
            ClipboardDataTransmitEvent event(mode, type, data, size);
            widget->clipboardDataTransmitEvent(&event);
        }
    }


    virtual void clipboardMetadataRecieveEvent(
        Window* window, const ClipboardMetadata &metadata, ClipboardMode mode
    )
    {
        auto widget = requestor(mode);
        if(widget)
        {
            ClipboardMetadataRecieveEvent event(mode, metadata);
            widget->clipboardMetadataRecieveEvent(&event);
        }
    }


    virtual void dndEnterEvent(Window* window, int x, int y)
    {
        cout << "dnd enter: " << x << ", " << y << "\n";
    }


    virtual void dndLeaveEvent(Window* window)
    {

    }


    virtual void dndMoveEvent(Window* window, int x, int y)
    {
        cout << "dnd move:  " << x << ", " << y << "\n";
    }


    virtual void dndDropEvent(Window* window)
    {
        cout << "drop\n";
    }


    virtual void closeEvent(Window* window)
    {
        auto d = (WindowWidgetData*) window->data();
        d->widget->closeEvent();
    }
};

WindowEvents_Widget g_events;


void Widget::show()
{
    if(!isWindow())
    {
        if(width() <= 0)
            setWidth(100);

        if(height() <= 0)
            setHeight(100);

        auto window = Window::newInstance(
            width(), height(), "", Window::Type::GL
        );
#ifdef R64FX_DEBUG
        if(!window)
        {
            cerr << "Widget: Failed to create window!\n";
            abort();
        }
#endif//R64FX_DEBUG

        auto painter = Painter::newInstance(window);
#ifdef R64FX_DEBUG
        if(!painter)
        {
            cerr << "Widget: Failed to create painter!\n";
            abort();
        }
#endif//R64FX_DEBUG

        auto d = new(nothrow) WindowWidgetData;
#ifdef R64FX_DEBUG
        if(!d)
        {
            cerr << "Widget: Failed to create WindowWidgetData!\n";
            abort();
        }
#endif//R64FX_DEBUG

        d->widget = this;
        d->painter = painter;

        window->setData(d);

        m_parent.window = window;
        m_flags |= R64FX_WIDGET_IS_WINDOW;
    }
    m_parent.window->show();
    m_parent.window->resize(width(), height());
    update();
}


void Widget::hide()
{
    if(isWindow())
    {
        m_parent.window->hide();
    }
}


void Widget::close()
{
    if(isWindow())
    {
        auto d = (WindowWidgetData*) m_parent.window->data();

        Painter::deleteInstance(d->painter);
        Window::deleteInstance(m_parent.window);
        delete d;
        m_parent.window = nullptr;
        m_flags &= ~R64FX_WIDGET_IS_WINDOW;
    }
}


Window* Widget::window() const
{
    return (Window*)(isWindow() ? m_parent.window : nullptr);
}


Window* Widget::rootWindow() const
{
    return root()->window();
}


bool Widget::isWindow() const
{
    return m_flags & R64FX_WIDGET_IS_WINDOW;
}


void Widget::setWindowTitle(std::string title)
{
    if(isWindow())
    {
        m_parent.window->setTitle(title);
    }
}


std::string Widget::windowTitle() const
{
    if(isWindow())
    {
        return m_parent.window->title();
    }
    else
    {
        return "";
    }
}

}//namespace r64fx