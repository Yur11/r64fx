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

    virtual Image* image() { return nullptr; }


    virtual void setTitle(std::string title);

    virtual std::string title();


    virtual void startTextInput();

    virtual void stopTextInput();

    virtual bool doingTextInput();


    virtual void anounceClipboardData(ClipboardMetadata* metadata, ClipboardMode mode);


    inline ::Window xWindow() const { return m_xwindow; }

    inline XIC inputContext() const { return m_input_context; }

    static WindowX11* getWindowFromXWindow(::Window xwindow);

    static unsigned int getEventButton(XButtonEvent* event);

    static void processSomeEvents(WindowEvents* events);

    void setupEvents();

    void sendSelection(const XSelectionRequestEvent &in);

    void recieveSelection(const XSelectionEvent &in, WindowEvents* events);

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

    ClipboardMetadata* g_metadata_clipboard     = nullptr;
    ClipboardMetadata* g_metadata_selection     = nullptr;
    ClipboardMetadata* g_metadata_drag_and_drop = nullptr;

    inline ClipboardMetadata* &g_metadata(ClipboardMode mode)
    {
        switch(mode)
        {
            case ClipboardMode::Clipboard:
                return g_metadata_clipboard;
            case ClipboardMode::Selection:
                return g_metadata_selection;
            default:
                return g_metadata_drag_and_drop;
        }
    }

    bool g_incoming_drag = false;
//     bool g_outgoing_drag = false;

}//namespace

#include "WindowX11_Atoms.cxx"
#include "WindowX11_Properties.cxx"
#include "WindowX11_XDND.cxx"

namespace{
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

            init_atoms();

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


#include "WindowX11_XImage.cxx"
#ifdef R64FX_USE_GL
#include "WindowX11_GLX.cxx"
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


Atom atom(ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Clipboard:
            return X11_Atom::CLIPBOARD;

        case ClipboardMode::Selection:
            return XA_PRIMARY;

        case ClipboardMode::DragAndDrop:
            return X11_Atom::XdndSelection;

        default:
            return None;
    }
}


void WindowX11::anounceClipboardData(ClipboardMetadata* metadata, ClipboardMode mode)
{
    Atom selection = atom(mode);
    if(selection == None)
    {
        cerr << "Bad ClipboardMode!\n";
        return;
    }

    XSetSelectionOwner(g_display, selection, m_xwindow, CurrentTime);
    g_metadata(mode) = metadata;
}


// bool WindowX11::hasSelection()
// {
//     return XGetSelectionOwner(g_display, XA_PRIMARY) == m_xwindow;
// }
//
//
// void WindowX11::requestSelection()
// {
//     XConvertSelection(
//         g_display,
//         XA_PRIMARY,
//         X11_Atom::TEXT,
//         X11_Atom::_R64FX_SELECTION,
//         m_xwindow,
//         CurrentTime
//     );
// }
//
//
// void WindowX11::setClipboardData(const std::string &text)
// {
//     g_clipboard_text = text;
//     XSetSelectionOwner(g_display, X11_Atom::CLIPBOARD, m_xwindow, CurrentTime);
// }
//
//
// void WindowX11::requestClipboardData()
// {
//     XConvertSelection(
//         g_display,
//         X11_Atom::CLIPBOARD,
//         X11_Atom::TARGETS,
//         X11_Atom::_R64FX_CLIPBOARD,
//         m_xwindow,
//         CurrentTime
//     );
// }


void WindowX11::processSomeEvents(WindowEvents* events)
{
//     if(g_drag_types_requested)
//     {
//         g_drag_types_requested = false;
//     }

    while(XPending(g_display))
    {
        XEvent xevent;
        XNextEvent(g_display, &xevent);
        auto xwindow = xevent.xany.window;

        WindowX11* window = getWindowFromXWindow(xwindow);
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
                    events->textInputEvent(window, str, XLookupKeysym(&xevent.xkey, 0));
                }
                else
                {
                    events->keyPressEvent(window, XLookupKeysym(&xevent.xkey, 0));
                }
                break;
            }

            case KeyRelease:
            {
                events->keyReleaseEvent(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case ButtonPress:
            {
                unsigned int button = getEventButton(&xevent.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mousePressEvent(window, xevent.xbutton.x, xevent.xbutton.y, button);
                }
                break;
            }

            case ButtonRelease:
            {
                unsigned int button = getEventButton(&xevent.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mouseReleaseEvent(window, xevent.xbutton.x, xevent.xbutton.y, button);
                }
                break;
            }

            case MotionNotify:
            {
                events->mouseMoveEvent(window, xevent.xmotion.x, xevent.xmotion.y);
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
                    events->resizeEvent(window, new_w, new_h);
                }
                break;
            }

            case SelectionClear:
            {
                break;
            }

            case SelectionRequest:
            {
                window->sendSelection(xevent.xselectionrequest);
                break;
            }

            case SelectionNotify:
            {
                window->recieveSelection(xevent.xselection, events);
                break;
            }

            case ClientMessage:
            {
                auto &msg = xevent.xclient;

                if(msg.message_type == X11_Atom::XdndPosition)
                {
                    cout << "XdndPosition\n";
                    int x, y;
                    get_dnd_position(msg.data.l, x, y);
                    cout << x << ", " << y << "\n";
                    if(!g_incoming_drag)
                    {
                        g_incoming_drag = true;
                        request_all_dnd_positions(xwindow, dnd_source(msg.data.l));
                    }
                }
                else if(msg.message_type == X11_Atom::XdndEnter)
                {
                    cout << "XdndEnter\n";
                    vector<Atom> types;
                    get_dnd_type_list(msg.data.l, types);
                    for(auto atom : types)
                    {
                        cout << atom << " -> " << atom_name(atom) << "\n";
                    }
                }
                else if(msg.message_type == X11_Atom::XdndLeave)
                {
                    cout << "XdndLeave\n";
                    g_incoming_drag = false;
                }
                else if(msg.message_type == X11_Atom::XdndDrop)
                {
                    cout << "XdndDrop\n";
                    send_dnd_finished(xwindow, dnd_source(msg.data.l), false);
                    g_incoming_drag = false;
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
            Atom targets[3] = {X11_Atom::TARGETS, X11_Atom::UTF8_STRING, X11_Atom::TEXT};

            XChangeProperty(
                g_display,
                in.requestor,
                in.property,
                XA_ATOM,
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
            cout << "Send selection!\n";

            string str = "Debugme!?";

            XChangeProperty(
                g_display,
                in.requestor,
                in.property,
                in.target,
                8,
                PropModeReplace,
                (unsigned char*) str.c_str(),
                str.size()
            );

            if(!XSendEvent(g_display, in.requestor, False, NoEventMask, &xevent))
            {
                cerr << "Failed to send selection event!\n";
            }
        }
    }
}


void WindowX11::recieveSelection(const XSelectionEvent &in, WindowEvents* events)
{
    if((in.selection == XA_PRIMARY || in.selection == X11_Atom::CLIPBOARD) &&
       (in.property == X11_Atom::_R64FX_SELECTION || in.property == X11_Atom::_R64FX_CLIPBOARD))
    {
        if(in.target == X11_Atom::TARGETS)
        {
            vector<Atom> atoms;
            if(get_window_atom_list_property(
                m_xwindow, in.property, XA_ATOM, true, atoms
            ))
            {
                cout << "atoms: " << atoms.size() << "\n";
                for(auto atom : atoms)
                {
                    cout << "    " << atom;
                    if(atom != None)
                    {
                        cout << " -> " << atom_name(atom);
                    }
                    cout << "\n";
                }

                Atom best_type = None;
                for(auto atom : atoms)
                {
                    if(atom == X11_Atom::UTF8_STRING)
                    {
                        if(best_type == None)
                        {
                            best_type = atom;
                            break;
                        }
                    }
                    else if(atom == X11_Atom::TEXT)
                    {
                        if(best_type == None)
                        {
                            best_type = atom;
                        }
                    }
                }

                if(best_type != None)
                {
                    cout << "requesting: " << atom_name(best_type) << "\n";

                    XConvertSelection(
                        g_display,
                        X11_Atom::CLIPBOARD,
                        best_type,
                        X11_Atom::_R64FX_CLIPBOARD,
                        m_xwindow,
                        CurrentTime
                    );
                }
                else
                {
                    cout << "Skip!\n";
                }
            }
            else
            {
                cerr << "Failed to get window property " << atom_name(in.property) << "\n";
            }
        }
        else if(in.target == X11_Atom::TEXT || in.target == X11_Atom::UTF8_STRING)
        {

            string text;
            if(get_window_text_property(
                m_xwindow, in.property, in.target, true, text
            ))
            {
//                 events->clipboard_input(this, text, (in.property == X11_Atom::_R64FX_SELECTION));
            }
            else
            {
                cerr << "Failed to get window property " << atom_name(in.property) << "\n";
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


void Window::processSomeEvents(WindowEvents* events)
{
    WindowX11::processSomeEvents(events);
}


void Window::forEach(void (*fun)(Window* window, void* data), void* data)
{
    for(auto window : g_all_windows)
        fun(window, data);
}

}//namespace r64fx