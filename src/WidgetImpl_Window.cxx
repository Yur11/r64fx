/* To be included in Widget.cpp */

namespace r64fx{

class WindowEventDispatcher : public WindowEventDispatcherIface{

    virtual void resizeEvent(Window* window, int width, int height)
    {
        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->setSize({width, height});
        d->m_root_widget->repaint();
    }


    virtual void mousePressEvent(Window* window, int x, int y, unsigned int button)
    {
        g_pressed_buttons |= MouseButton(button);

        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->initMousePressEvent(Point<int>(x, y), MouseButton(button));

        g_prev_mouse_position = Point<int>(x, y);
    }


    virtual void mouseReleaseEvent(Window* window, int x, int y, unsigned int button)
    {
        g_pressed_buttons &= ~MouseButton(button);

        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->initMouseReleaseEvent(Point<int>(x, y), MouseButton(button));
    }


    virtual void mouseMoveEvent(Window* window, int x, int y)
    {
        Point<int> position(x, y);
        Point<int> delta = position - g_prev_mouse_position;

        auto d = (WidgetImpl*) window->data();
        g_moused_over_widget = d->m_root_widget->initMouseMoveEvent(position, delta, g_pressed_buttons, g_moused_over_widget);

        g_prev_mouse_position = position;
    }


    virtual void mouseEnterEvent(Window* window)
    {
//         d->widget->mouseEnterEvent();
    }


    virtual void mouseLeaveEvent(Window* window)
    {
        if(g_moused_over_widget)
        {
            g_moused_over_widget->initMouseLeaveEvent();
        }
    }


    virtual void keyPressEvent(Window* window, unsigned int key)
    {
        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->initKeyPressEvent(key);
    }


    virtual void keyReleaseEvent(Window* window, unsigned int key)
    {
        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->initKeyReleaseEvent(key);
    }


    virtual void textInputEvent(Window* window, const std::string &text, unsigned int key)
    {
        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->initTextInputEvent(text, key);
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
        auto d = (WidgetImpl*) window->data();
        d->m_root_widget->closeEvent();
    }
};

long int g_window_count = 0;


void Widget::show(
    Window::WmType  wm_type,
    Window::Type    type,
    Window*         modal_parent
)
{
    /* Create new window if needed. */
    if(!isWindow())
    {
        if(width() <= 0)
            setWidth(100);

        if(height() <= 0)
            setHeight(100);

        auto window = Window::newInstance(
            width(), height(), type, wm_type
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

        auto d = new(std::nothrow) WidgetImpl;
        if(d)
        {
            d->m_window = window;
            d->m_root_widget = this;
            d->m_painter = painter;

            window->setData(d);

            m_parent.window = window;
            m_flags |= R64FX_WIDGET_IS_WINDOW;

            g_window_count++;

            /* Setup gui timer if needed. */
            if(!g_gui_timer)
            {
                g_gui_timer = new (std::nothrow) Timer;
                if(g_gui_timer)
                {
                    g_gui_timer->setInterval(1000);
                    g_gui_timer->onTimeout([](Timer* timer, void* data){

                        WindowEventDispatcher events;
                        Window::processSomeEvents(&events);

                        Window::forEach([](Window* window, void* data){
                            auto impl = (WidgetImpl*) window->data();
                            impl->initPaintCycle();
                        }, nullptr);

                    }, nullptr);
                    g_gui_timer->start();
                }
#ifdef R64FX_DEBUG
                else
                {
                    cerr << "Widget: Failed to create gui timer!\n";
                }
#endif//R64FX_DEBUG
            }
        }
#ifdef R64FX_DEBUG
        else
        {
            cerr << "Widget: Failed to create WidgetImpl!\n";
        }
#endif//R64FX_DEBUG
    }
    m_parent.window->setSize(size());
    if(modal_parent)
        m_parent.window->setModalTo(modal_parent);
    m_parent.window->show();
    repaint();
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
//         auto d = (WindowWidgetData*) m_parent.window->data();

//         Painter::deleteInstance(d->painter);
        m_parent.window->hide();
        Window::deleteInstance(m_parent.window);
//         delete d;
        m_parent.window = nullptr;
        m_flags &= ~R64FX_WIDGET_IS_WINDOW;

        g_window_count--;
        if(g_window_count == 0)
        {
            if(g_gui_timer)
            {
                g_gui_timer->stop();
                delete g_gui_timer;
            }
            else
            {
#ifdef R64FX_DEBUG
                cerr << "Widget: gui timer is null!\n";
#endif//R64FX_DEBUG
            }
        }
#ifdef R64FX_DEBUG
        else if(g_window_count < 0)
        {
            cerr << "Widget: Window count is less than zero!\n";
        }
#endif//R64FX_DEBUG
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