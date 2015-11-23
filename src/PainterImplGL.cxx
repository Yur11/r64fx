/* To be included in Panter.cpp */

namespace{
    int PainterImplGL_count = 0;

    bool gl_stuff_is_good = false;

    Shader_rgba* g_Shader_rgba = nullptr;
}


struct PainterImplGL : public PainterImpl{
    VertexArray_rgba* m_VertexArray_rgba = nullptr;

    PainterImplGL(Window* window);

    virtual ~PainterImplGL();

    virtual void debugDraw();

    virtual void repaint();

    virtual void prepare();

    virtual void clear();

    static void initGLStuffIfNeeded();

    static void cleanupGLStuff();

};//PainterImplGL


PainterImpl* create_gl_painter(Window* window)
{
    return new PainterImplGL(window);
}


PainterImplGL::PainterImplGL(Window* window) : PainterImpl(window)
{
    initGLStuffIfNeeded();
    PainterImplGL_count++;
}


PainterImplGL::~PainterImplGL()
{
    cout << "~PainterImplGL\n";

    PainterImplGL_count--;
    if(PainterImplGL_count == 0)
    {
        cleanupGLStuff();
    }
#ifdef R64FX_DEBUG
    else if(PainterImplGL_count <= 0)
    {
        cerr << "Warning PainterImplGL_count is " << PainterImplGL_count << "!\n";
        cerr << "Something is really wrong!\n";
    }
#endif//R64FX_DEBUG
}


void PainterImplGL::debugDraw()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::ClearColor(1.0, 1.0, 1.0, 0.0);
    gl::Clear(GL_COLOR_BUFFER_BIT);
    g_Shader_rgba->use();
    g_Shader_rgba->setScaleAndShift(
        1.0f/float(window->width()),
       -1.0f/float(window->height()),
       -1.0f,
        1.0f
    );
//     g_Shader_rgba->debugDraw();
    window->repaint();
}


void PainterImplGL::repaint()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::Clear(GL_COLOR_BUFFER_BIT);
    window->repaint();
}


void PainterImplGL::prepare()
{
//     if(m_VertexArray_rgba)
//     {
//         if(m_VertexArray_rgba->vertexCount() < (int)paint_commands.size() * 4)
//         {
//             delete m_VertexArray_rgba;
//             m_VertexArray_rgba = nullptr;
//         }
//     }
//
//     if(!m_VertexArray_rgba)
//     {
//         m_VertexArray_rgba = new VertexArray_rgba(g_Shader_rgba, m_PaintCommands_FillRect.size() * 4);
//     }
}


void PainterImplGL::clear()
{
    PainterImpl::clear();
}


void PainterImplGL::initGLStuffIfNeeded()
{
    if(gl_stuff_is_good)
        return;

    int major, minor;
    gl::GetIntegerv(GL_MAJOR_VERSION, &major);
    gl::GetIntegerv(GL_MINOR_VERSION, &minor);
    cout << "gl: " << major << "." << minor << "\n";

    gl::InitIfNeeded();
    gl::ClearColor(1.0, 1.0, 1.0, 0.0);

    g_Shader_rgba = new Shader_rgba;
    if(!g_Shader_rgba->isOk())
        abort();

    gl_stuff_is_good = true;
}


void PainterImplGL::cleanupGLStuff()
{
    cout << "CleanupGLStuff\n";

    if(!gl_stuff_is_good)
        return;

    if(g_Shader_rgba)
        delete g_Shader_rgba;
}


void PaintCommandImpl_FillRect::prepareGL(PainterImplGL* impl)
{

}


void PaintCommandImpl_PutImage::prepareGL(PainterImplGL* impl)
{

}


void PaintCommandImpl_PutDensePlotHorizontal::prepareGL(PainterImplGL* impl)
{

}


void PaintCommandImpl_PutDensePlotVertical::prepareGL(PainterImplGL* impl)
{

}
