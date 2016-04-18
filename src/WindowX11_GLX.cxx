/* To be included in WindowX11.cpp */

namespace{
    int g_glx_major = 0;
    int g_glx_minor = 0;
    GLXContext g_first_gl_context = 0;
}//namespace


struct WindowGLX : public WindowX11{
    GLXContext m_gl_context = 0;

    virtual ~WindowGLX() {}

    virtual Window::Type type() { return Window::Type::GL; }

    virtual void setup(int width, int height);

    virtual void cleanup();

    virtual void makeCurrent();

    virtual void repaint(Rect<int>* rects, int numrects);
};


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

    updateAttrs();

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
        g_first_gl_context, //Existing Context
        True,
        context_attribs
    );

    if(!g_first_gl_context)
    {
        g_first_gl_context = m_gl_context;
    }

    makeCurrent();
    XFree(glxfbconfigs);
    setupEvents();
    setupInputContext();
}


void WindowGLX::cleanup()
{
    if(m_gl_context == g_first_gl_context)
    {
        g_first_gl_context = 0;
    }
    glXDestroyContext(g_display, m_gl_context);
}


void WindowGLX::makeCurrent()
{
    glXMakeCurrent(g_display, m_xwindow, m_gl_context);
}


void WindowGLX::repaint(Rect<int>* rects, int numrects)
{
    glXSwapBuffers(g_display, m_xwindow);
}