#include "WindowX11.hpp"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <vector>
#include <iostream>
#include "Image.hpp"

using namespace std;

namespace r64fx{

extern vector<Window*> g_all_windows;

namespace{

Display*   g_display = nullptr;
bool       g_got_x_error = false;
int        g_screen;
Atom       g_WM_PROTOCOLS;
Atom       g_WM_DELETE_WINDOW;
Atom       g_NET_WM_NAME;
Atom       g_UTF8_STRING;


int x_error_handler(Display* display, XErrorEvent* event)
{
    g_got_x_error = true;

    cerr << "Got X Error!\nmajor: " << int(event->request_code) << ", minor: " << int(event->minor_code) << "\n";
    const int buff_length = 1024;
    static char buff[buff_length];
    XGetErrorText(display, event->error_code, buff, buff_length);
    cout << buff << "\n";

    abort();

    return 0;
}


WindowX11* get_window_from_xwindow(::Window xwindow)
{
    for(unsigned int i=0; i<g_all_windows.size(); i++)
    {
        WindowX11* window = (WindowX11*) g_all_windows[i];
        if(window->xWindow() == xwindow)
        {
            return window;
        }
    }
    return nullptr;
}

#ifdef R64FX_USE_GL
struct FBConfig{
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

    FBConfig(){}

    FBConfig(Display* display, GLXFBConfig* cfg)
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

    FBConfig(const FBConfig &other)
    {
        operator=(other);
    }

    FBConfig &operator=(const FBConfig &other)
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
};

bool operator>(const FBConfig &a, const FBConfig &b)
{
    return
        a.samples        > b.samples        &&
        a.sample_buffers > b.sample_buffers &&
        a.red            > b.red            &&
        a.green          > b.green          &&
        a.blue           > b.blue           &&
        a.alpha          > b.alpha          &&
        a.depth          > b.depth          &&
        a.double_buffer  > b.double_buffer
    ;
}

bool operator==(const FBConfig &a, const FBConfig &b)
{
    return
        a.samples        == b.samples        &&
        a.sample_buffers == b.sample_buffers &&
        a.red            == b.red            &&
        a.green          == b.green          &&
        a.blue           == b.blue           &&
        a.alpha          == b.alpha          &&
        a.depth          == b.depth          &&
        a.double_buffer  == b.double_buffer
    ;
}

FBConfig g_fbconfig;
#endif//R64FX_USE_GL

}//namespace


WindowX11::WindowX11(Window::Type type)
: Window(type)
{
    if(type == Window::Type::Normal)
    {
        m_xwindow = XCreateSimpleWindow(
            g_display,
            RootWindow(g_display, g_screen),
            0, 0, 640, 480, 0,
            None, None
        );

        updateAttrs();
        XGetWindowAttributes(g_display, m_xwindow, attrs());

        m_xgc_values.graphics_exposures = True;
        m_gc = XCreateGC(g_display, m_xwindow, GCGraphicsExposures, &(m_xgc_values));

        resizeImage();
    }
#ifdef R64FX_USE_GL
    else if(type == Window::Type::GL)
    {
        cerr << "Blaaa!\n";

        int major = 0;
        int minor = 0;
        if(!glXQueryVersion(g_display, &major, &minor))
        {
            cerr << "Failed to get glx version!\n";
            abort();
        }

        cout << "GLX: " << major << "." << minor << "\n";

        int nglxfbconfigs = 0;
        GLXFBConfig* glxfbconfigs = glXGetFBConfigs(g_display, g_screen, &nglxfbconfigs);
        if(!glxfbconfigs || nglxfbconfigs < 1)
        {
            cerr << "Failed to get GLX FB configs!\n";
            abort();
        }

        vector<FBConfig> best_cfgs;
        FBConfig best_cfg;
        for(int i=0; i<nglxfbconfigs; i++)
        {
            FBConfig cfg(g_display, glxfbconfigs + i);
            if(cfg > best_cfg)
            {
                best_cfg = cfg;
                best_cfgs.clear();
                best_cfgs.push_back(cfg);
            }
            else if(cfg == best_cfg)
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
            RootWindow(g_display, g_screen),
            0, 0, 640, 480, 0,
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
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        m_gl_context = glXCreateContextAttribsARB(
            g_display,
            best_cfg.config[0],
            0, //Exising Context
            True,
            context_attribs
        );

        cout << m_gl_context << "\n";

        makeCurrent();
    }
#endif//R64FX_USE_GL
    else
    {
        cerr << "Window::Window: Window type not implemented!\n";
        abort();
    }

    XSetWMProtocols(g_display, m_xwindow, &g_WM_DELETE_WINDOW, 1);
    XSelectInput(
        g_display, m_xwindow,
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        ExposureMask
    );
}


WindowX11::~WindowX11()
{

}


Window* WindowX11::newWindow(int width, int height, std::string title, Window::Type type)
{
    if(!g_display)
    {
        XSetErrorHandler(x_error_handler);

        g_display = XOpenDisplay(nullptr);
        if(!g_display)
        {
            cerr << "Failed to open display!\n";
            return nullptr;
        }

        g_WM_PROTOCOLS       = XInternAtom(g_display, "WM_PROTOCOLS",       False);
        g_WM_DELETE_WINDOW   = XInternAtom(g_display, "WM_DELETE_WINDOW",   False);
        g_NET_WM_NAME        = XInternAtom(g_display, "_NET_WM_NAME",       False);
        g_UTF8_STRING        = XInternAtom(g_display, "UTF8_STRING",        False);
    }

    return new WindowX11(type);
}


void WindowX11::cleanup()
{
    XCloseDisplay(g_display);
}


void WindowX11::show()
{
    XMapWindow(g_display, m_xwindow);
    XFlush(g_display);
}


void WindowX11::hide()
{
    XUnmapWindow(g_display, m_xwindow);
    XFlush(g_display);
}


void WindowX11::resize(int width, int height)
{
    XResizeWindow(g_display, m_xwindow, width, height);
}


void WindowX11::makeCurrent()
{
    if(type() == Window::Type::GL)
    {
        glXMakeCurrent(g_display, m_xwindow, m_gl_context);
    }
}


void WindowX11::repaint()
{
    if(type() == Window::Type::Normal)
    {
        XPutImage(
            g_display,
            m_xwindow,
            m_gc,
            m_ximage,
            0, 0,
            0, 0,
            m_ximage->width,
            m_ximage->height
        );

        XFlush(g_display);
    }
    else if(type() == Window::Type::GL)
    {
        glXSwapBuffers(g_display, m_xwindow);
    }
}


Image* WindowX11::image() const
{
    return m_image;
}


void WindowX11::setTitle(std::string title)
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


std::string WindowX11::title() const
{
    return m_title;
}


int WindowX11::width() const
{
    return m_attrs.width;
}


int WindowX11::height() const
{
    return m_attrs.width;
}


void WindowX11::processSomeEvents(Window::Events* events)
{
    while(XPending(g_display))
    {

        XEvent xevent;
        XNextEvent(g_display, &xevent);

        WindowX11* window = get_window_from_xwindow(xevent.xany.window);
        if(!window)
            continue;

        switch(xevent.type)
        {
            case KeyPress:
            {
                events->key_press(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case KeyRelease:
            {
                events->key_release(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case ButtonPress:
            {
                break;
            }

            case ButtonRelease:
            {
                break;
            }

            case MotionNotify:
            {
                break;
            }

            case Expose:
            {
                window->processExposeEvent();
                events->resize(window, window->width(), window->height());
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
                cout << "Unknown Event!\n";
                break;
            }
        }
    }
}


void WindowX11::updateAttrs()
{
    XGetWindowAttributes(g_display, m_xwindow, &m_attrs);
}


XWindowAttributes* WindowX11::attrs()
{
    return &m_attrs;
}


void WindowX11::resizeImage()
{
    destroyImage();

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

    char* buff = new char[m_attrs.width * m_attrs.height * 4];
    m_ximage = XCreateImage(
        g_display,
        m_attrs.visual,
        vinfo.depth,  //Bits per pixel.
        ZPixmap,
        0,            //Offset
        buff,
        m_attrs.width,
        m_attrs.height,
        32,
        0              //Bytes Per Line. Xlib will calculate.
    );

    m_image = new Image(m_ximage->width, m_ximage->height, 4, (unsigned char*)m_ximage->data);
}


void WindowX11::destroyImage()
{
    if(!m_ximage)
        return;

    XDestroyImage(m_ximage);
    m_ximage = nullptr;
    delete m_image;
    m_image = nullptr;
}


void WindowX11::processExposeEvent()
{
    updateAttrs();
    if(type() == Window::Type::Normal)
    {
        resizeImage();
    }
}


void WindowX11::createGLContext()
{

}


void WindowX11::destroyGLContext()
{

}

}//namespace r64fx