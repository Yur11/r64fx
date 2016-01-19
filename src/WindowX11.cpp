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
#include <iostream>

using namespace std;

namespace r64fx{

class WindowX11;

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

    namespace X11_Atom{
        Atom WM_PROTOCOLS;
        Atom WM_DELETE_WINDOW;
        Atom _NET_WM_NAME;
        Atom UTF8_STRING;
        Atom TEXT;
        Atom TARGETS;
        Atom MULTIPLE;
        Atom _R64FX_SELECTION;
        Atom CLIPBOARD;
        Atom _R64FX_CLIPBOARD;
    }

    bool intern_atom(const char* name, Atom &atom, bool only_if_exists)
    {
        atom = XInternAtom(g_display, name, only_if_exists);
        if(atom == None)
        {
            cerr << "Failed to intern atom " << name << "\n";
            return false;
        }
        return true;
    }

    string g_selection_text = "";
    string g_clipboard_text = "";

#ifdef R64FX_USE_MITSHM
    int g_mitshm_major = 0;
    int g_mitshm_minor = 0;
    int g_mitshm_has_pixmaps = 0;
    int g_mitsm_completion_event = 0;

    inline bool got_mitshm() { return g_mitshm_major > 0; }

    inline bool got_mitshm_pixmaps() { return g_mitshm_has_pixmaps; }
#endif//R64FX_USE_MITSHM

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

#define R64FX_INTERN_ATOM(name, only_if_exists) intern_atom(#name, X11_Atom::name, only_if_exists)

            R64FX_INTERN_ATOM( WM_PROTOCOLS,     true  );
            R64FX_INTERN_ATOM( WM_DELETE_WINDOW, true  );
            R64FX_INTERN_ATOM( _NET_WM_NAME,     true  );
            R64FX_INTERN_ATOM( UTF8_STRING,      true  );
            R64FX_INTERN_ATOM( TEXT,             true  );
            R64FX_INTERN_ATOM( TARGETS,          true  );
            R64FX_INTERN_ATOM( MULTIPLE,         true  );
            R64FX_INTERN_ATOM( _R64FX_SELECTION, false );
            R64FX_INTERN_ATOM( CLIPBOARD,        true  );
            R64FX_INTERN_ATOM( _R64FX_CLIPBOARD, false );

//             g_WM_PROTOCOLS       = XInternAtom(g_display, "WM_PROTOCOLS",       True);
//             g_WM_DELETE_WINDOW   = XInternAtom(g_display, "WM_DELETE_WINDOW",   True);
//             g_NET_WM_NAME        = XInternAtom(g_display, "_NET_WM_NAME",       True);
//             g_UTF8_STRING        = XInternAtom(g_display, "UTF8_STRING",        True);
//             g_TEXT               = XInternAtom(g_display, "TEXT",               True);
//             g_TARGETS            = XInternAtom(g_display, "TARGETS",            True);
//             g_MULTIPLE           = XInternAtom(g_display, "MULTIPLE",           True);
//             g_R64FX_SELECTION    = XInternAtom(g_display, "_R64FX_SELECTION", False);

#ifdef R64FX_USE_MITSHM
            XShmQueryVersion(g_display, &g_mitshm_major, &g_mitshm_minor, &g_mitshm_has_pixmaps);
            g_mitsm_completion_event = XShmGetEventBase(g_display) + ShmCompletion;
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

#ifdef R64FX_USE_GL
    int g_glx_major = 0;
    int g_glx_minor = 0;
#endif//R64FX_USE_GL
}//namespace


struct WindowX11 : public Window, public LinkedList<WindowX11>::Node{
    WindowX11*         m_window;
    ::Window           m_xwindow;
    XWindowAttributes  m_attrs;
    string             m_title;
    XIC                m_input_context;

    virtual ~WindowX11() {}

    virtual void setup(int width, int height) = 0;

    virtual void cleanup() = 0;

    virtual void show();

    virtual void hide();

    virtual void resize(int width, int height);

    virtual int width();

    virtual int height();

    virtual void setTitle(std::string title);

    virtual Image* image() { return nullptr; }

    virtual std::string title();

    virtual void startTextInput();

    virtual void stopTextInput();

    virtual bool doingTextInput();

    virtual void setSelection(const std::string &text);

    virtual bool hasSelection();

    virtual void requestSelection();

    virtual void setClipboardData(const std::string &text);

    virtual void requestClipboardData();

    inline ::Window xWindow() const { return m_xwindow; }

    inline XIC inputContext() const { return m_input_context; }

    static WindowX11* getWindowFromXWindow(::Window xwindow);

    static unsigned int getEventButton(XButtonEvent* event);

    static void processSomeEvents(Window::Events* events);

    void setupEvents();

    void sendSelection(const XSelectionRequestEvent &in);

    void recieveSelection(const XSelectionEvent &in, Window::Events* events);

    void updateAttrs();

    void setupInputContext();
};


#include "WindowXImage.cxx"
#ifdef R64FX_USE_GL
#include "WindowGLX.cxx"
#endif//R64FX_USE_GL


void WindowX11::show()
{
    XMapWindow(g_display, m_xwindow);
}


void WindowX11::hide()
{
    XUnmapWindow(g_display, m_xwindow);
}


void WindowX11::resize(int width, int height)
{
    XResizeWindow(g_display, m_xwindow, width, height);
}


int WindowX11::width()
{
    return m_attrs.width;
}


int WindowX11::height()
{
    return m_attrs.height;
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


void WindowX11::setSelection(const std::string &text)
{
    g_selection_text = text;
    XSetSelectionOwner(g_display, XA_PRIMARY, m_xwindow, CurrentTime);
}


bool WindowX11::hasSelection()
{
    return XGetSelectionOwner(g_display, XA_PRIMARY) == m_xwindow;
}


void WindowX11::requestSelection()
{
    XConvertSelection(
        g_display,
        XA_PRIMARY,
        X11_Atom::TEXT,
        X11_Atom::_R64FX_SELECTION,
        m_xwindow,
        CurrentTime
    );
}


void WindowX11::setClipboardData(const std::string &text)
{
    g_clipboard_text = text;
    XSetSelectionOwner(g_display, X11_Atom::CLIPBOARD, m_xwindow, CurrentTime);
}


void WindowX11::requestClipboardData()
{
    XConvertSelection(
        g_display,
        XA_PRIMARY,
        X11_Atom::TEXT,
        X11_Atom::_R64FX_CLIPBOARD,
        m_xwindow,
        CurrentTime
    );
}


void WindowX11::processSomeEvents(Window::Events* events)
{
    while(XPending(g_display))
    {
        XEvent xevent;
        XNextEvent(g_display, &xevent);

        WindowX11* window = getWindowFromXWindow(xevent.xany.window);
        if(!window)
        {
            continue;
        }

        if(window->doingTextInput() && XFilterEvent(&xevent, xevent.xany.window))
        {
            continue;
        }

        switch(xevent.type)
        {
            case KeyPress:
            {
                if(window->doingTextInput())
                {
                    Status status;
                    char buff[8];
                    KeySym keysym;
                    int nbytes = Xutf8LookupString(
                        window->inputContext(), &xevent.xkey,
                        buff, 8, &keysym, &status
                    );

                    string str = "";
                    if(nbytes)
                    {
                        str = string(buff, nbytes);
                    }
                    events->text_input(window, str, XLookupKeysym(&xevent.xkey, 0));
                }
                else
                {
                    events->key_press(window, XLookupKeysym(&xevent.xkey, 0));
                }
                break;
            }

            case KeyRelease:
            {
                events->key_release(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case ButtonPress:
            {
                unsigned int button = getEventButton(&xevent.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mouse_press(window, xevent.xbutton.x, xevent.xbutton.y, button);
                }
                break;
            }

            case ButtonRelease:
            {
                unsigned int button = getEventButton(&xevent.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mouse_release(window, xevent.xbutton.x, xevent.xbutton.y, button);
                }
                break;
            }

            case MotionNotify:
            {
                events->mouse_move(window, xevent.xmotion.x, xevent.xmotion.y);
                break;
            }

            case ConfigureNotify:
            {
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
                    events->resize(window, new_w, new_h);
                }
                break;
            }

            case SelectionClear:
            {
                cout << "SelectionClear\n";
                cout << "\n";
                break;
            }

            case SelectionRequest:
            {
                cout << "SelectionRequest\n";
                window->sendSelection(xevent.xselectionrequest);
                cout << "\n";
                break;
            }

            case SelectionNotify:
            {
                cout << "SelectionNotify\n";
                window->recieveSelection(xevent.xselection, events);
                cout << "\n";
                break;
            }

            case ClientMessage:
            {
                if(xevent.xclient.message_type == X11_Atom::WM_PROTOCOLS)
                {
                    events->close(window);
                }
                break;
            }

            default:
            {
#ifdef R64FX_USE_MITSHM
                if(xevent.type == g_mitsm_completion_event)
                {
                    cout << "MitShm Completion Event!\n";
                }
                else
#endif//R64FX_USE_MITSHM
                {
                    cout << "Unknown Event!\n";
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
        StructureNotifyMask
    );
}


void WindowX11::sendSelection(const XSelectionRequestEvent &in)
{
    if(in.property == None)
    {
        cout << "property None\n";
        return;
    }

    if(in.selection == XA_PRIMARY || in.selection == X11_Atom::CLIPBOARD)
    {
        XEvent xevent;
        auto &out = xevent.xselection;
        out.type      = SelectionNotify;
        out.display   = in.display;
        out.requestor = in.requestor;
        out.selection = in.selection;
        out.target    = in.target;
        out.property  = in.property;
        out.time      = in.time;

        if(in.target == X11_Atom::TARGETS)
        {
            cout << "tagets\n";
            Atom targets[3] = {X11_Atom::TARGETS, X11_Atom::TEXT};

            XChangeProperty(
                g_display,
                in.requestor,
                in.property,
                in.target,
                32,
                PropModeReplace,
                (unsigned char*) targets,
                3
            );

            if(!XSendEvent(g_display, in.requestor, False, NoEventMask, &xevent))
            {
                cerr << "Failed to send selection event!\n";
            }
        }
        else if(in.target == X11_Atom::TEXT || in.target == X11_Atom::UTF8_STRING)
        {
            string* str = nullptr;
            if(in.selection == XA_PRIMARY)
            {
                str = &g_selection_text;
            }
            else
            {
                str = &g_clipboard_text;
            }

            XChangeProperty(
                g_display,
                in.requestor,
                in.property,
                in.target,
                8,
                PropModeReplace,
                (unsigned char*) str->c_str(),
                str->size()
            );

            if(!XSendEvent(g_display, in.requestor, False, NoEventMask, &xevent))
            {
                cerr << "Failed to send selection event!\n";
            }
        }
        else
        {
            cout << "other!\n";
            char* str = XGetAtomName(g_display, in.target);
            cout << str << "\n";
            XFree(str);
        }
    }
}


void WindowX11::recieveSelection(const XSelectionEvent &in, Window::Events* events)
{
    if(in.selection == XA_PRIMARY)
    {
        if(in.target == X11_Atom::TARGETS)
        {
        }
        else if(in.target == X11_Atom::TEXT)
        {
            if(in.property == X11_Atom::_R64FX_SELECTION || in.property == X11_Atom::_R64FX_CLIPBOARD)
            {
                Atom            type;
                int             format;
                unsigned long   nitems;
                unsigned long   bytes_after;
                unsigned char*  data;

                /* Get property length. */
                int result = XGetWindowProperty(
                    g_display, m_xwindow, in.property,
                    0, 0, False,
                    X11_Atom::TEXT,
                    &type, &format, &nitems, &bytes_after,
                    &data
                );

                if(result != Success)
                {
                    cerr << "Failed to get window property size!\n";
                    return;
                }

                if(format == 8 && bytes_after > 0)
                {
                    int size = bytes_after;
                    while(size % 4)
                        size++;
                    size /= 4;

                    result = XGetWindowProperty(
                        g_display, m_xwindow, in.property,
                        0, size, True,
                        X11_Atom::TEXT,
                        &type, &format, &nitems, &bytes_after,
                        &data
                    );

                    if(result != Success)
                    {
                        cerr << "Failed to get window property data!\n";
                        return;
                    }

                    string text((char*)data);
                    events->clipboard_input(this, text, (in.property == X11_Atom::_R64FX_SELECTION));
                    XFree(data);
                }
                else
                {
                    cerr << "Unsupported text property format!\n";
                }
            }
            else
            {
                cout << "OTHER\n";
            }
        }
    }
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
    std::string title,
    Window::Type type
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
        window->setTitle(title);
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


void Window::processSomeEvents(Window::Events* events)
{
    WindowX11::processSomeEvents(events);
}


void Window::forEach(void (*fun)(Window* window, void* data), void* data)
{
    for(auto window : g_all_windows)
        fun(window, data);
}

}//namespace r64fx