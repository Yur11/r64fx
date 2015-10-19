#include "WindowX11.hpp"
#include "Image.hpp"

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

namespace{
    Display* g_display = nullptr;
    int      g_screen;

    int x_error_handler(Display*, XErrorEvent*);

    Atom g_WM_PROTOCOLS;
    Atom g_WM_DELETE_WINDOW;
    Atom g_NET_WM_NAME;
    Atom g_UTF8_STRING;

#ifdef R64FX_USE_MITSHM
    int g_mitshm_major = 0;
    int g_mitshm_minor = 0;
    int g_mitshm_has_pixmaps = 0;
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

            g_WM_PROTOCOLS       = XInternAtom(g_display, "WM_PROTOCOLS",       False);
            g_WM_DELETE_WINDOW   = XInternAtom(g_display, "WM_DELETE_WINDOW",   False);
            g_NET_WM_NAME        = XInternAtom(g_display, "_NET_WM_NAME",       False);
            g_UTF8_STRING        = XInternAtom(g_display, "UTF8_STRING",        False);

#ifdef R64FX_USE_MITSHM
            XShmQueryVersion(g_display, &g_mitshm_major, &g_mitshm_minor, &g_mitshm_has_pixmaps);
#endif//R64FX_USE_MITSHM
        }
    }

#ifdef R64FX_USE_MITSHM
    inline bool got_mitshm() { return g_mitshm_major < 0; }

    inline bool got_mitshm_pixmaps() { return g_mitshm_has_pixmaps; }
#endif//R64FX_USE_MITSHM


    /* === Error Handler === */
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


    /* === Private Structures === */
    struct WindowX11Private;
    vector<WindowX11Private*> g_privates;

    struct WindowX11Private{
        WindowX11*         window;
        ::Window           xwindow;
        XWindowAttributes  attrs;
        string             title = "";

        WindowX11Private(WindowX11* window) : window(window)
        {
            g_privates.push_back(this);
        }

        ~WindowX11Private()
        {
            for(auto it=g_privates.begin(); it!=g_privates.end(); it++)
            {
                if(this == *it)
                {
                    g_privates.erase(it);
                    return;
                }
            }
        }

        void updateAttrs()
        {
            XGetWindowAttributes(g_display, xwindow, &attrs);
        }

        void setTitle(string title)
        {
            XTextProperty xtp;
            char* strlst[] = { (char*)title.c_str() };
            Status status = XStringListToTextProperty(strlst, 1, &xtp);
            if(status)
            {
                XSetTextProperty(g_display, xwindow, &xtp, XA_WM_NAME);
                title = title;
            }
        }

        void setupEvents()
        {
            XSetWMProtocols(g_display, xwindow, &g_WM_DELETE_WINDOW, 1);
            XSelectInput(
                g_display, xwindow,
                KeyPressMask | KeyReleaseMask |
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                ExposureMask
            );
        }

        void show()
        {
            XMapWindow(g_display, xwindow);
            XFlush(g_display);
        }

        void hide()
        {
            XUnmapWindow(g_display, xwindow);
            XFlush(g_display);
        }

        void resize(int width, int height)
        {
            XResizeWindow(g_display, xwindow, width, height);
        }
    };


    struct WindowX11PrivateNormal : public WindowX11Private{
        Image*           image   = nullptr;
        XImage*          ximage  = nullptr;
#ifdef R64FX_USE_MITSHM
        XShmSegmentInfo* shminfo = nullptr;
#endif//R64FX_USE_MITSHM
        XGCValues        xgc_values;
        GC               gc;

        WindowX11PrivateNormal(WindowX11* window)
        : WindowX11Private(window)
        {
            xwindow = XCreateSimpleWindow(
                g_display,
                RootWindow(g_display, g_screen),
                0, 0, 640, 480, 0,
                None, None
            );

            updateAttrs();

            xgc_values.graphics_exposures = True;
            gc = XCreateGC(g_display, xwindow, GCGraphicsExposures, &xgc_values);

            resizeImage();
        }

        void resizeImage()
        {
            destroyImage();

            XVisualInfo vinfo;
            vinfo.visualid = XVisualIDFromVisual(attrs.visual);
            {
                int nitems = 0;
                XVisualInfo* vinfos = XGetVisualInfo(g_display, VisualIDMask, &vinfo, &nitems);
                if(vinfos)
                {
                    vinfo = vinfos[0];
                    XFree(vinfos);
                }
            }

#ifdef R64FX_USE_MITSHM
            if(got_mitshm())
            {
                shminfo = new XShmSegmentInfo;
                ximage = XShmCreateImage(
                    g_display,
                    attrs.visual,
                    vinfo.depth,
                    ZPixmap,
                    nullptr,
                    shminfo,
                    attrs.width,
                    attrs.height
                );
                shminfo->shmid = shmget(IPC_PRIVATE, ximage->bytes_per_line * ximage->height, IPC_CREAT|0777);
                shminfo->shmaddr = ximage->data = (char*) shmat(shminfo->shmid, 0, 0);
            }
            else
#endif//R64FX_USE_MITSHM
            {
                char* buff = new char[attrs.width * attrs.height * 4];
                ximage = XCreateImage(
                    g_display,
                    attrs.visual,
                    vinfo.depth,  //Bits per pixel.
                    ZPixmap,
                    0,            //Offset
                    buff,
                    attrs.width,
                    attrs.height,
                    32,
                    0              //Bytes Per Line. Xlib will calculate.
                );
            }

            image = new Image(ximage->width, ximage->height, 4, (unsigned char*)ximage->data);
        }

        void destroyImage()
        {
            if(!ximage)
                return;

#ifdef R64FX_USE_MITSHM
            if(got_mitshm())
            {
                XShmDetach(g_display, shminfo);
                shmdt(shminfo->shmaddr);
                shmctl(shminfo->shmid, IPC_RMID, 0);
            }
#endif//R64FX_USE_MITSHM
            XDestroyImage(ximage);
            ximage = nullptr;
            delete image;
            image = nullptr;
        }

        void repaint()
        {
#ifdef R64FX_USE_MITSHM
            if(got_mitshm())
            {
                XShmPutImage(
                    g_display,
                    xwindow,
                    gc,
                    ximage,
                    0, 0,
                    0, 0,
                    ximage->width,
                    ximage->height,
                    True
                );
            }
            else
#endif//R64FX_USE_MITSHM
            {
                XPutImage(
                    g_display,
                    xwindow,
                    gc,
                    ximage,
                    0, 0,
                    0, 0,
                    ximage->width,
                    ximage->height
                );
            }

            XFlush(g_display);
        }

        void processExposeEvent()
        {
            updateAttrs();
            resizeImage();
        }
    };


#ifdef R64FX_USE_GL
    int g_glx_major = 0;
    int g_glx_minor = 0;

    struct WindowX11PrivateGLX : public WindowX11Private{
        GLXContext gl_context = 0;

        WindowX11PrivateGLX(WindowX11* window)
        : WindowX11Private(window)
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
            GLXFBConfig* glxfbconfigs = glXGetFBConfigs(g_display, g_screen, &nglxfbconfigs);
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

            xwindow = XCreateWindow(
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

            gl_context = glXCreateContextAttribsARB(
                g_display,
                best_cfg.config[0],
                0, //Exising Context
                True,
                context_attribs
            );

            makeCurrent();

            XFree(glxfbconfigs);
        }

        void swapBuffers()
        {
            glXSwapBuffers(g_display, xwindow);
        }

        void makeCurrent()
        {
            glXMakeCurrent(g_display, xwindow, gl_context);
        }
    };
#endif//R64FX_USE_GL


    WindowX11Private* get_window_from_xwindow(::Window xwindow)
    {
        for(unsigned int i=0; i<g_privates.size(); i++)
        {
            auto p = (WindowX11Private*) g_privates[i];
            if(p->xwindow == xwindow)
            {
                return p;
            }
        }
        return nullptr;
    }
}//namespace


/* === Public Interface === */
WindowX11::WindowX11(Window::Type type)
: Window(type)
{
    if(type == Window::Type::Normal)
    {
        m_private = new WindowX11PrivateNormal(this);
    }
#ifdef R64FX_USE_GL
    else if(type == Window::Type::GL)
    {
        m_private = new WindowX11PrivateGLX(this);
    }
#endif//R64FX_USE_GL
    else
    {
        cerr << "Window::Window: Window type not implemented!\n";
        abort();
    }

    auto p = (WindowX11Private*) m_private;
    p->setupEvents();
}


WindowX11::~WindowX11()
{
    if(m_private)
    {
        if(type() == Window::Type::Normal)
        {
            auto p = (WindowX11PrivateNormal*) m_private;
            delete p;
        }
#ifdef R64FX_USE_GL
        else if(type() == Window::Type::GL)
        {
            auto p = (WindowX11PrivateGLX*) m_private;
            delete p;
        }
#endif//R64FX_USE_GL
    }
}


Window* WindowX11::newWindow(int width, int height, std::string title, Window::Type type)
{
    init_x_if_needed();
    return new WindowX11(type);
}


void WindowX11::cleanup()
{
    XCloseDisplay(g_display);
}


void WindowX11::show()
{
    auto p = (WindowX11Private*) m_private;
    p->show();
}


void WindowX11::hide()
{
    auto p = (WindowX11Private*) m_private;
    p->hide();
}


void WindowX11::resize(int width, int height)
{
    auto p = (WindowX11Private*) m_private;
    p->resize(width, height);
}


void WindowX11::makeCurrent()
{
#ifdef R64FX_USE_GL
    if(type() == Window::Type::GL)
    {
        auto p = (WindowX11PrivateGLX*) m_private;
        p->makeCurrent();
    }
#endif//R64FX_USE_GL
}


void WindowX11::repaint()
{
    if(type() == Window::Type::Normal)
    {
        auto p = (WindowX11PrivateNormal*) m_private;
        p->repaint();
    }
#ifdef R64FX_USE_GL
    else if(type() == Window::Type::GL)
    {
        auto p = (WindowX11PrivateGLX*) m_private;
        p->swapBuffers();
    }
#endif//R64FX_USE_GL
}


Image* WindowX11::image() const
{
    auto p = (WindowX11PrivateNormal*) m_private;
    return p->image;
}


void WindowX11::setTitle(std::string title)
{
    auto p = (WindowX11Private*) m_private;
    p->setTitle(title);
}


std::string WindowX11::title() const
{
    auto p = (WindowX11Private*) m_private;
    return p->title;
}


int WindowX11::width() const
{
    auto p = (WindowX11Private*) m_private;
    return p->attrs.width;
}


int WindowX11::height() const
{
    auto p = (WindowX11Private*) m_private;
    return p->attrs.width;
}


void WindowX11::processSomeEvents(Window::Events* events)
{
    while(XPending(g_display))
    {
        XEvent xevent;
        XNextEvent(g_display, &xevent);

        WindowX11Private* wp = get_window_from_xwindow(xevent.xany.window);
        if(!wp)
            continue;
        auto window = wp->window;

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
                if(window->type() == Window::Type::Normal)
                {
                    auto p = (WindowX11PrivateNormal*) wp;
                    p->processExposeEvent();
                }
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

}//namespace r64fx