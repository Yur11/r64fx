/* To be included in WindowX11.cpp */

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