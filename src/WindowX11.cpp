#include "LinkedList.hpp"
#include "Image.hpp"
#include "MouseButtonCodes.hpp"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#ifdef R64FX_USE_MITSHM
#include <X11/extensions/XShm.h>
#include <sys/shm.h>
#endif//R64FX_USE_MITSHM

#ifdef R64FX_USE_GL
#include <GL/glx.h>
#endif//R64FX_USE_GL

#include <vector>
#include <map>
#include <iostream>

using namespace std;

namespace r64fx{

struct WindowX11 : public Window, public LinkedList<WindowX11>::Node{
    WindowX11*         m_window;
    ::Window           m_xwindow;
    XWindowAttributes  m_attrs;
    string             m_title;
    XIC                m_input_context;

    int mx = 0;
    int my = 0;

    virtual ~WindowX11() {}

    virtual void setup(int width, int height) = 0;

    virtual void cleanup() = 0;

    virtual void show();

    virtual void hide();

    virtual void setPosition(Point<int> position);

    virtual Point<int> position();

    virtual void setSize(Size<int> size);

    virtual Size<int> size();

    virtual Size<int> getScreenSize();


    virtual Image* image() { return nullptr; }


    virtual void setTitle(std::string title);

    virtual std::string title();

    virtual void setWmType(Window::WmType wm_type);

    virtual void showDecorations(bool yes);

    virtual void setModalTo(Window* window);


    virtual void grabMouse();

    virtual void ungrabMouse();


    virtual void startTextInput();

    virtual void stopTextInput();

    virtual bool doingTextInput();


    virtual void anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode);

    virtual void requestClipboardData(ClipboardDataType type, ClipboardMode mode);

    virtual void requestClipboardMetadata(ClipboardMode mode);


    virtual void startDrag(Window* drag_object, int anchor_x, int anchor_y);


    inline ::Window xWindow() const { return m_xwindow; }

    inline XIC inputContext() const { return m_input_context; }

    static WindowX11* getWindowFromXWindow(::Window xwindow);

    static unsigned int getEventButton(XButtonEvent* event);

    static void processSomeEvents(WindowEventDispatcherIface* events);

    void setupEvents();

    void sendSelection(WindowEventDispatcherIface* events);

    void onSelectionTransmit(void* data, int size);

    void recieveSelection(const XSelectionEvent &in, WindowEventDispatcherIface* events);

    void clearSelection(const XSelectionClearEvent &in);

    void updateAttrs();

    void setupInputContext();
};


namespace{
    LinkedList<WindowX11> g_all_windows;

    Display* g_display = nullptr;

    bool g_got_x_error = false;

    int x_error_handler(Display* display, XErrorEvent* event)
    {
        g_got_x_error = true;

        cerr << "Got X Error!\nmajor: " << int(event->request_code) << ", minor: " << int(event->minor_code) << "\n";
        const int buff_length = 1024;
        static char buff[buff_length];
        XGetErrorText(display, event->error_code, buff, buff_length);
        cerr << buff << "\n";

        abort();

        return 0;
    }

    XIM  g_input_method = 0;

    WindowX11* g_text_input_grabber = nullptr;

    bool g_incoming_drag = false;

    Window* g_outgoing_drag_object = nullptr;
    int g_drag_anchor_x = 0;
    int g_drag_anchor_y = 0;

    XEvent g_incoming_event;
    XEvent g_outgoing_event;
}//namespace


#include "WindowX11_Atoms.cxx"
#include "WindowX11_Properties.cxx"
#include "WindowX11_Clipboard.cxx"
#include "WindowX11_XDND.cxx"
#include "WindowX11_XImage.cxx"
#ifdef R64FX_USE_GL
#include "WindowX11_GLX.cxx"
#endif//R64FX_USE_GL


namespace{
    void init_x_if_needed()
    {
        if(!g_display)
        {
            XSetErrorHandler(x_error_handler);

            g_display = XOpenDisplay(nullptr);
            if(!g_display)
            {
                cerr << "Failed to open display!\n";
                abort();
            }

            g_input_method = XOpenIM(g_display, nullptr, nullptr, nullptr);
            if(!g_input_method)
            {
                cerr << "Failed to get input method!\n";
                abort();
            }

            init_atoms();
            init_clipboard();
#ifdef R64FX_USE_MITSHM
            init_mitshm();
#endif//R64FX_USE_MITSHM
        }
    }

    void cleanup_x_if_empty()
    {
        if(!g_display)
            return;

        if(g_all_windows.isEmpty())
        {
            if(g_input_method)
            {
                XCloseIM(g_input_method);
                g_input_method = nullptr;
            }

            XCloseDisplay(g_display);
            g_display = 0;
        }
    }
}//namespace


void WindowX11::show()
{
    XMapWindow(g_display, m_xwindow);
}


void WindowX11::hide()
{
    XUnmapWindow(g_display, m_xwindow);
}


void WindowX11::setPosition(Point<int> position)
{
    XMoveWindow(g_display, m_xwindow, position.x(), position.y());
}


Point<int> WindowX11::position()
{
    return {mx, my};
}


void WindowX11::setSize(Size<int> size)
{
    XResizeWindow(g_display, m_xwindow, size.width(), size.height());
}


Size<int> WindowX11::size()
{
    return {m_attrs.width, m_attrs.height};
}


Size<int> WindowX11::getScreenSize()
{
    XWindowAttributes attrs;
    XGetWindowAttributes(g_display, DefaultRootWindow(g_display), &attrs);
    return Size<int>(attrs.width, attrs.height);
}


void WindowX11::setTitle(string title)
{
    XTextProperty xtp;
    char* strlst[] = { (char*)title.c_str() };
    Status status = XStringListToTextProperty(strlst, 1, &xtp);
    if(status)
    {
        XSetTextProperty(g_display, m_xwindow, &xtp, XA_WM_NAME);
        m_title = title;
    }
}


std::string WindowX11::title()
{
    return m_title;
}


void WindowX11::setWmType(Window::WmType wm_type)
{
    Atom wm_type_atom = X11_Atom::_NET_WM_WINDOW_TYPE_NORMAL;

    switch(wm_type)
    {
        case Window::WmType::Normal:
            return;

        case Window::WmType::Menu:
        {
            wm_type_atom = X11_Atom::_NET_WM_WINDOW_TYPE_MENU;
            break;
        }

        case Window::WmType::Dialog:
        {
            wm_type_atom = X11_Atom::_NET_WM_WINDOW_TYPE_DIALOG;
            break;
        }

        case Window::WmType::ToolTip:
        {
            wm_type_atom = X11_Atom::_NET_WM_WINDOW_TYPE_TOOLTIP;
            break;
        }

        case Window::WmType::DND:
        {
            wm_type_atom = X11_Atom::_NET_WM_WINDOW_TYPE_DND;
            break;
        }

        default:
            break;
    }

    XChangeProperty(
        g_display,
        m_xwindow,
        X11_Atom::_NET_WM_WINDOW_TYPE,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&wm_type_atom,
        1
    );

    if(wm_type != Window::WmType::Normal)
    {
        XSetWindowAttributes attrs;
        attrs.override_redirect = True;

        XChangeWindowAttributes(
            g_display,
            m_xwindow,
            CWOverrideRedirect,
            &attrs
        );
    }
}


void WindowX11::showDecorations(bool yes)
{
    struct{
        unsigned long  flags   = (1L << 1);
        unsigned long  functions       = 0;
        unsigned long  decorarations;
        long           input_mode      = 0;
        unsigned long  status          = 0;
    } motif_hints;
    motif_hints.decorarations = (yes ? 1 : 0);

    XChangeProperty(
        g_display,
        m_xwindow,
        X11_Atom::_MOTIF_WM_HINTS,
        X11_Atom::_MOTIF_WM_HINTS,
        32,
        PropModeReplace,
        (unsigned char*) &motif_hints,
        5
    );
}


void WindowX11::setModalTo(Window* window)
{
    auto window_x11 = (WindowX11*) window;

    XChangeProperty(
        g_display,
        m_xwindow,
        X11_Atom::WM_TRANSIENT_FOR,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&(window_x11->m_xwindow),
        1
    );
}


void WindowX11::grabMouse()
{
    XGrabPointer(
        g_display,
        m_xwindow,
        False,
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        EnterWindowMask | LeaveWindowMask,
        GrabModeAsync, GrabModeAsync, None, None,
        CurrentTime
    );
}


void WindowX11::ungrabMouse()
{
    XUngrabPointer(
        g_display,
        CurrentTime
    );
}


void WindowX11::startTextInput()
{
    XSetICFocus(m_input_context);
    g_text_input_grabber = this;
}


void WindowX11::stopTextInput()
{
    if(doingTextInput())
    {
        XUnsetICFocus(m_input_context);
        g_text_input_grabber = nullptr;
    }
}


bool WindowX11::doingTextInput()
{
    return g_text_input_grabber == this;
}


void WindowX11::processSomeEvents(WindowEventDispatcherIface* events)
{
    while(XPending(g_display))
    {
        g_incoming_event = XEvent();
        XNextEvent(g_display, &g_incoming_event);
        auto xwindow = g_incoming_event.xany.window;

        WindowX11* window = getWindowFromXWindow(xwindow);
        if(!window)
        {
            continue;
        }

        if(window->doingTextInput() && XFilterEvent(&g_incoming_event, g_incoming_event.xany.window))
        {
            continue;
        }

        switch(g_incoming_event.type)
        {
            case KeyPress:
            {
                if(window->doingTextInput())
                {
                    Status status;
                    char buff[8];
                    KeySym keysym;
                    int nbytes = Xutf8LookupString(
                        window->inputContext(), &g_incoming_event.xkey,
                        buff, 8, &keysym, &status
                    );

                    string str = "";
                    if(nbytes)
                    {
                        str = string(buff, nbytes);
                    }
                    events->textInputEvent(window, str, XLookupKeysym(&g_incoming_event.xkey, 0));
                }
                else
                {
                    events->keyPressEvent(window, XLookupKeysym(&g_incoming_event.xkey, 0));
                }
                break;
            }

            case KeyRelease:
            {
                events->keyReleaseEvent(window, XLookupKeysym(&g_incoming_event.xkey, 0));
                break;
            }

            case ButtonPress:
            {
                unsigned int button = getEventButton(&g_incoming_event.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mousePressEvent(window, g_incoming_event.xbutton.x, g_incoming_event.xbutton.y, button);
                }
                break;
            }

            case ButtonRelease:
            {
                unsigned int button = getEventButton(&g_incoming_event.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    if(g_outgoing_drag_object)
                    {
                        window->ungrabMouse();
                        events->dndFinished();
                        g_outgoing_drag_object = nullptr;
                        g_drag_anchor_x = 0;
                        g_drag_anchor_y = 0;
                    }
                    else
                    {
                        events->mouseReleaseEvent(window, g_incoming_event.xbutton.x, g_incoming_event.xbutton.y, button);
                    }
                }
                break;
            }

            case MotionNotify:
            {
                if(g_outgoing_drag_object)
                {
                    int dnd_obj_x = g_incoming_event.xmotion.x + window->x() - g_drag_anchor_x;
                    int dnd_obj_y = g_incoming_event.xmotion.y + window->y() - g_drag_anchor_y;
                    g_outgoing_drag_object->setPosition({dnd_obj_x, dnd_obj_y});
                }
                else
                {
                    events->mouseMoveEvent(window, g_incoming_event.xmotion.x, g_incoming_event.xmotion.y);
                }
                break;
            }

            case EnterNotify:
            {
                events->mouseEnterEvent(window);
                break;
            }

            case LeaveNotify:
            {
                events->mouseLeaveEvent(window);
                break;
            }

            case ConfigureNotify:
            {
                window->mx = g_incoming_event.xconfigure.x;
                window->my = g_incoming_event.xconfigure.y;

                int old_w = window->width();
                int old_h = window->height();
                window->updateAttrs();
                int new_w = window->width();
                int new_h = window->height();

                if(old_w != new_w || old_h != new_h)
                {
                    if(window->type() == Window::Type::Image)
                    {
                        auto windowximage = (WindowXImage*) window;
                        windowximage->resizeImage();
                    }
                    events->resizeEvent(window, new_w, new_h);
                }
                break;
            }

            case SelectionClear:
            {
                window->clearSelection(g_incoming_event.xselectionclear);
                break;
            }

            case SelectionRequest:
            {
                window->sendSelection(events);
                break;
            }

            case SelectionNotify:
            {
                window->recieveSelection(g_incoming_event.xselection, events);
                break;
            }

            case ClientMessage:
            {
                auto &msg = g_incoming_event.xclient;

                if(msg.message_type == X11_Atom::XdndPosition)
                {
                    int x, y;
                    get_dnd_position(msg.data.l, x, y);
                    if(!g_incoming_drag)
                    {
                        g_incoming_drag = true;
                        window->updateAttrs();
                        request_all_dnd_positions(xwindow, dnd_source(msg.data.l));
                        events->dndEnterEvent(window, x - window->x(), y - window->y());
                    }
                    else
                    {
                        events->dndMoveEvent(window, x - window->x(), y - window->y());
                    }
                }
                else if(msg.message_type == X11_Atom::XdndEnter)
                {
                    vector<Atom> types;
                    get_dnd_type_list(msg.data.l, types);
                }
                else if(msg.message_type == X11_Atom::XdndLeave)
                {
                    g_incoming_drag = false;
                    events->dndLeaveEvent(window);
                }
                else if(msg.message_type == X11_Atom::XdndDrop)
                {
                    send_dnd_finished(xwindow, dnd_source(msg.data.l), false);
                    g_incoming_drag = false;
                    events->dndDropEvent(window);
                }
                else if(msg.message_type == X11_Atom::XdndFinished)
                {
                    cout << "XdndFinished\n";
                }
                else if(msg.message_type == X11_Atom::WM_PROTOCOLS)
                {
                    events->closeEvent(window);
                }
                break;
            }

            default:
            {
#ifdef R64FX_USE_MITSHM
                if(g_incoming_event.type == g_mitsm_completion_event)
                {
                    cout << "MitShm Completion Event!\n";
                }
                else
#endif//R64FX_USE_MITSHM
                {
//                     cout << "Unknown Event!\n";
                }
                break;
            }
        }
    }
}


void WindowX11::setupEvents()
{
    XSetWMProtocols(g_display, m_xwindow, &X11_Atom::WM_DELETE_WINDOW, 1);

    XSelectInput(
        g_display, m_xwindow,
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        EnterWindowMask | LeaveWindowMask |
        StructureNotifyMask
    );

    unsigned int dnd_version = 5;
    XChangeProperty(
        g_display,
        m_xwindow,
        X11_Atom::XdndAware,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&dnd_version,
        1
    );
}


void WindowX11::updateAttrs()
{
    XGetWindowAttributes(g_display, m_xwindow, &m_attrs);
}


WindowX11* WindowX11::getWindowFromXWindow(::Window xwindow)
{
    for(auto window : g_all_windows)
    {
        auto windowx11 = (WindowX11*) window;
        if(windowx11->xWindow() == xwindow)
            return windowx11;
    }
    return nullptr;
}


unsigned int WindowX11::getEventButton(XButtonEvent* event)
{
    switch(event->button)
    {
        case Button1:
            return R64FX_MOUSE_BUTTON1;

        case Button2:
            return R64FX_MOUSE_BUTTON2;

        case Button3:
            return R64FX_MOUSE_BUTTON3;

        case Button4:
            return R64FX_MOUSE_BUTTON4;

        case Button5:
            return R64FX_MOUSE_BUTTON5;

        default:
            return R64FX_MOUSE_BUTTON_NONE;
    }
}


void WindowX11::setupInputContext()
{
    m_input_context = XCreateIC(
        g_input_method,
        XNInputStyle,     XIMPreeditNothing | XIMStatusNothing,
        XNClientWindow,   m_xwindow,
        (char*)nullptr
    );

    if(!m_input_context)
    {
        cerr << "Failed to create input context!\n";
        abort();
    }
}


Window* Window::newInstance(
    int width, int height,
    Window::Type     type,
    Window::WmType   wm_type
)
{
    init_x_if_needed();

    WindowX11* window = nullptr;
    if(type == Window::Type::Image)
    {
        window = new WindowXImage;
    }
#ifdef R64FX_USE_GL
    else if(type == Window::Type::GL)
    {
        window = new WindowGLX;
    }
#endif//R64FX_USE_GL

    if(window)
    {
        window->setup(width, height);
        window->setWmType(wm_type);
        window->setTitle("r64fx");
        g_all_windows.append(window);
    }

    return window;
}


void Window::deleteInstance(Window* window)
{
    auto windowx11 = dynamic_cast<WindowX11*>(window);
    if(!windowx11)
    {
        cerr << "Window::deleteInstance(): Bad window!\n";
        abort();
    }

    g_all_windows.remove(windowx11);
    windowx11->cleanup();
    delete windowx11;

    cleanup_x_if_empty();
}


void Window::processSomeEvents(WindowEventDispatcherIface* events)
{
    WindowX11::processSomeEvents(events);
}


void Window::forEach(void (*fun)(Window* window, void* data), void* data)
{
    for(auto window : g_all_windows)
        fun(window, data);
}

}//namespace r64fx