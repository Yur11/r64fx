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

    Atom g_WM_PROTOCOLS;
    Atom g_WM_DELETE_WINDOW;
    Atom g_NET_WM_NAME;
    Atom g_UTF8_STRING;

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

            g_WM_PROTOCOLS       = XInternAtom(g_display, "WM_PROTOCOLS",       False);
            g_WM_DELETE_WINDOW   = XInternAtom(g_display, "WM_DELETE_WINDOW",   False);
            g_NET_WM_NAME        = XInternAtom(g_display, "_NET_WM_NAME",       False);
            g_UTF8_STRING        = XInternAtom(g_display, "UTF8_STRING",        False);

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

    inline ::Window xWindow() const { return m_xwindow; }

    inline XIC inputContext() const { return m_input_context; }

    static WindowX11* getWindowFromXWindow(::Window xwindow);

    static unsigned int getEventButton(XButtonEvent* event);

    static void processSomeEvents(Window::Events* events);

    void setupEvents();

    void updateAttrs();

    void setupInputContext();
};


struct WindowXImage : public WindowX11{
    Image*           m_image   = nullptr;
    XGCValues        m_xgc_values;
    GC               m_gc;
    unsigned int     m_depth;

    virtual ~WindowXImage() {};

    virtual Window::Type type() { return Window::Type::Image; }

    virtual void setup(int width, int height);

    virtual void cleanup();

    virtual Image* image();

    virtual void makeCurrent() {};

    virtual void repaint(Rect<int>* rects, int numrects);

    void resizeImage();

    void destroyImage();
};


#ifdef R64FX_USE_GL
struct WindowGLX : public WindowX11{
    GLXContext m_gl_context = 0;

    virtual ~WindowGLX() {}

    virtual Window::Type type() { return Window::Type::GL; }

    virtual void setup(int width, int height);

    virtual void cleanup();

    virtual void makeCurrent();

    virtual void repaint(Rect<int>* rects, int numrects);
};
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


void WindowX11::processSomeEvents(Window::Events* events)
{
    while(XPending(g_display))
    {
        XEvent xevent;
        XNextEvent(g_display, &xevent);

        if(XFilterEvent(&xevent, xevent.xany.window))
        {
            continue;
        }

        WindowX11* window = getWindowFromXWindow(xevent.xany.window);
        if(!window)
        {
            continue;
        }

        switch(xevent.type)
        {
            case KeyPress:
            {
                Status status;
                char buff[8];
                KeySym key;
                int nbytes = Xutf8LookupString(
                    window->inputContext(), &xevent.xkey,
                    buff, 8, &key, &status
                );
                string str(buff, nbytes);
                cout << str << "\n";
//                 events->key_press(window, key);
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
                    events->resize(window, old_w, old_h, new_w, new_h);
                }
                break;
            }

            case ClientMessage:
            {
                if(xevent.xclient.message_type == g_WM_PROTOCOLS)
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
    XSetWMProtocols(g_display, m_xwindow, &g_WM_DELETE_WINDOW, 1);

    XSelectInput(
        g_display, m_xwindow,
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        StructureNotifyMask
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

    XSetICFocus(m_input_context);
}


void WindowXImage::setup(int width, int height)
{
    m_xwindow = XCreateSimpleWindow(
        g_display,
        RootWindow(g_display, XDefaultScreen(g_display)),
        0, 0, width, height, 0,
        None, None
    );

    m_xgc_values.graphics_exposures = True;
    m_gc = XCreateGC(g_display, m_xwindow, GCGraphicsExposures, &m_xgc_values);

    updateAttrs();

    XVisualInfo vinfo;
    vinfo.visualid = XVisualIDFromVisual(m_attrs.visual);
    {
        int nitems = 0;
        XVisualInfo* vinfos = XGetVisualInfo(g_display, VisualIDMask, &vinfo, &nitems);
        if(vinfos)
        {
            vinfo = vinfos[0];
            XFree(vinfos);
        }
    }
    m_depth = vinfo.depth;

    resizeImage();
    setupEvents();
    setupInputContext();
}


void WindowXImage::cleanup()
{
    destroyImage();
}


Image* WindowXImage::image()
{
    return m_image;
}


void WindowXImage::repaint(Rect<int>* rects, int numrects)
{
    Rect<int> extra_rect = {
        0, 0, m_image->width(), m_image->height()
    };

    if(!rects)
    {
        rects = &extra_rect;
        numrects = 1;
    }

    static unsigned long int masks[4] = {
        0xFF,
        0xFF00,
        0xFF0000,
        0xFF000000
    };

    for(int i=0; i<numrects; i++)
    {
        auto rect = rects[i];

        XImage* ximage = XCreateImage(
            g_display, m_attrs.visual, m_depth, ZPixmap, 0,
            (char*)m_image->data(), m_image->width(), m_image->height(),
            32, m_image->width() * 4
        );

        struct {
            unsigned char r = 0, g = 0, b = 0, a = 0;
        } component_index;

        for(int i=0; i<4; i++)
        {
            if(masks[i] == ximage->red_mask)
            {
                component_index.r = i;
            }
            else if(masks[i] == ximage->green_mask)
            {
                component_index.g = i;
            }
            else if(masks[i] == ximage->blue_mask)
            {
                component_index.b = i;
            }
            else
            {
                component_index.a = i;
            }
        }

        for(int y=0; y<rect.height(); y++)
        {
            for(int x=0; x<rect.width(); x++)
            {
                auto px = m_image->pixel(x + rect.x(), y + rect.y());
                unsigned char r = px[0];
                unsigned char g = px[1];
                unsigned char b = px[2];
                px[component_index.r] = r;
                px[component_index.g] = g;
                px[component_index.b] = b;
            }
        }

        XPutImage(
            g_display, m_xwindow, m_gc, ximage,
            0, 0, 0, 0, ximage->width, ximage->height
        );

        ximage->data = 0; //Lifted this from Qt4 code.
                            //Apparently you can do this to avoid buffer deallocation.
        XDestroyImage(ximage);

        XSync(g_display, False);
    }
}


void WindowXImage::resizeImage()
{
    destroyImage();
    m_image = new Image(width(), height(), 4);
}


void WindowXImage::destroyImage()
{
    if(m_image)
    {
        delete m_image;
        m_image = nullptr;
    }
}


#ifdef R64FX_USE_GL
void WindowGLX::setup(int width, int height)
{
    if(g_glx_major == 0)
    {
        if(!glXQueryVersion(g_display, &g_glx_major, &g_glx_minor))
        {
            cerr << "Failed to get glx version!\n";
            abort();
        }
    }


    struct FrameBufferConfig{
        GLXFBConfig* config = nullptr;
        int samples  = 0;
        int sample_buffers = 0;
        int red = 0;
        int green = 0;
        int blue = 0;
        int alpha = 0;
        int depth = 0;
        int double_buffer = 0;
        int render_type = 0;
        int visual_type = 0;

        FrameBufferConfig(){}

        FrameBufferConfig(Display* display, GLXFBConfig* cfg)
        {
            config = cfg;
            glXGetFBConfigAttrib(display, *cfg, GLX_SAMPLES,        &samples);
            glXGetFBConfigAttrib(display, *cfg, GLX_SAMPLE_BUFFERS, &sample_buffers);
            glXGetFBConfigAttrib(display, *cfg, GLX_RED_SIZE,       &red);
            glXGetFBConfigAttrib(display, *cfg, GLX_GREEN_SIZE,     &green);
            glXGetFBConfigAttrib(display, *cfg, GLX_BLUE_SIZE,      &blue);
            glXGetFBConfigAttrib(display, *cfg, GLX_ALPHA_SIZE,     &alpha);
            glXGetFBConfigAttrib(display, *cfg, GLX_DEPTH_SIZE,     &depth);
            glXGetFBConfigAttrib(display, *cfg, GLX_DOUBLEBUFFER,   &double_buffer);
            glXGetFBConfigAttrib(display, *cfg, GLX_RENDER_TYPE,    &render_type);
            glXGetFBConfigAttrib(display, *cfg, GLX_X_VISUAL_TYPE,  &visual_type);
        }

        FrameBufferConfig(const FrameBufferConfig &other)
        {
            operator=(other);
        }

        FrameBufferConfig &operator=(const FrameBufferConfig &other)
        {
            config         = other.config;
            samples        = other.samples;
            sample_buffers = other.sample_buffers;
            red            = other.red;
            green          = other.green;
            blue           = other.blue;
            alpha          = other.alpha;
            depth          = other.depth;
            double_buffer  = other.double_buffer;
            render_type    = other.render_type;
            visual_type    = other.visual_type;

            return *this;
        }


        bool isBetterThan(const FrameBufferConfig &other)
        {
            return
                samples        > other.samples        &&
                sample_buffers > other.sample_buffers &&
                red            > other.red            &&
                green          > other.green          &&
                blue           > other.blue           &&
                alpha          > other.alpha          &&
                depth          > other.depth          &&
                double_buffer  > other.double_buffer
            ;
        }

        bool isAtLeastAsGoodAs(const FrameBufferConfig &other)
        {
            return
                samples        == other.samples        &&
                sample_buffers == other.sample_buffers &&
                red            == other.red            &&
                green          == other.green          &&
                blue           == other.blue           &&
                alpha          == other.alpha          &&
                depth          == other.depth          &&
                double_buffer  == other.double_buffer
            ;
        }
    };

    int nglxfbconfigs = 0;
    GLXFBConfig* glxfbconfigs = glXGetFBConfigs(g_display, XDefaultScreen(g_display), &nglxfbconfigs);
    if(!glxfbconfigs || nglxfbconfigs < 1)
    {
        cerr << "Failed to get GLX FB configs!\n";
        abort();
    }

    vector<FrameBufferConfig> best_cfgs;
    FrameBufferConfig best_cfg;
    for(int i=0; i<nglxfbconfigs; i++)
    {
        FrameBufferConfig cfg(g_display, glxfbconfigs + i);
        if(cfg.isBetterThan(best_cfg))
        {
            best_cfg = cfg;
            best_cfgs.clear();
            best_cfgs.push_back(cfg);
        }
        else if(cfg.isAtLeastAsGoodAs(best_cfg))
        {
            best_cfgs.push_back(cfg);
        }
    }

    if(best_cfgs.empty())
    {
        cerr << "Failed to get good GLX FB configs!\n";
        abort();
    }

    XVisualInfo* vinfo = glXGetVisualFromFBConfig(g_display, best_cfg.config[0]);

    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(
        g_display,
        RootWindow(g_display, vinfo->screen),
        vinfo->visual,
        AllocNone
    );
    swa.background_pixmap = None;
    swa.border_pixel      = 0;
    swa.event_mask        = StructureNotifyMask;

    m_xwindow = XCreateWindow(
        g_display,
        RootWindow(g_display, XDefaultScreen(g_display)),
        0, 0, width, height, 0,
        vinfo->depth, InputOutput, vinfo->visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &swa
    );

    XFree(vinfo);

    show();

    typedef GLXContext (*glXCreateContextAttribsARBFun)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
    auto glXCreateContextAttribsARB = (glXCreateContextAttribsARBFun) glXGetProcAddressARB(
        (const GLubyte*)"glXCreateContextAttribsARB"
    );

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        None
    };

    m_gl_context = glXCreateContextAttribsARB(
        g_display,
        best_cfg.config[0],
        0, //Exising Context
        True,
        context_attribs
    );

    makeCurrent();
    XFree(glxfbconfigs);
    setupEvents();
    setupInputContext();
}


void WindowGLX::cleanup()
{

}


void WindowGLX::makeCurrent()
{
    glXMakeCurrent(g_display, m_xwindow, m_gl_context);
}


void WindowGLX::repaint(Rect<int>* rects, int numrects)
{
    glXSwapBuffers(g_display, m_xwindow);
}
#endif//R64FX_USE_GL


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


void Window::forEachWindow(void (*fun)(Window* window, void* data), void* data)
{
    for(auto window : g_all_windows)
        fun(window, data);
}

}//namespace r64fx