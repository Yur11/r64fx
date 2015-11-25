/* To be included in Panter.cpp */

namespace{
    int PainterImplGL_count = 0;

    bool gl_stuff_is_good = false;

    Shader_rgba*      g_Shader_rgba      = nullptr;
    Shader_rgba_tex*  g_Shader_rgba_tex  = nullptr;

    const GLuint primitive_restart = 0xFFFF;
}


class PaintLayer;

struct PainterImplGL : public PainterImpl{
    GLuint base_texture = 0;

    vector<PaintLayer*> layers;

    PainterImplGL(Window* window);

    virtual ~PainterImplGL();

    virtual void configure();

    void traverseCommands(LinkedList<PaintCommand>::Iterator begin, LinkedList<PaintCommand>::Iterator end);

    void resizeBaseTexture(int w, int h);

    void deleteBaseTextureIfNeeded();

    virtual void repaint();

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
    resizeBaseTexture(window->width(), window->height());

    PainterImplGL_count++;
}


PainterImplGL::~PainterImplGL()
{
    deleteBaseTextureIfNeeded();

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


void PainterImplGL::configure()
{
    int w = window->width();
    while(w && 3)
        w++;
    resizeBaseTexture(w, window->height());

    if(root_group)
    {
        traverseCommands(root_group->commands.begin(), root_group->commands.end());
    }
}


void PainterImplGL::traverseCommands(LinkedList<PaintCommand>::Iterator begin, LinkedList<PaintCommand>::Iterator end)
{
    for(auto it=begin; it!=end; ++it)
    {
        auto pc = *it;
        Image img(pc->rect.width(), pc->rect.height(), 4);
        target_image = &img;
        int x = pc->rect.x();
        int y = pc->rect.y();
        pc->rect.setX(0);
        pc->rect.setY(0);
        pc->paint(this);
        pc->rect.setX(x);
        pc->rect.setY(y);
        target_image = nullptr;
    }
}


void PainterImplGL::resizeBaseTexture(int w, int h)
{
    cout << "resize tex: " << w << "x" << h << "\n";

    deleteBaseTextureIfNeeded();

    gl::GenTextures(1, &base_texture);
    gl::BindTexture(GL_TEXTURE_2D, base_texture);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl::TexStorage2D(
        GL_TEXTURE_2D,
        1,
        GL_RGBA8,
        w, h
    );
}


void PainterImplGL::deleteBaseTextureIfNeeded()
{
    if(base_texture != 0)
        gl::DeleteTextures(1, &base_texture);
}


void PainterImplGL::repaint()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::Clear(GL_COLOR_BUFFER_BIT);

    //Draw base texture here!

    window->repaint();
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

    g_Shader_rgba_tex = new Shader_rgba_tex;
    if(!g_Shader_rgba_tex->isOk())
        abort();

    gl::Enable(GL_PRIMITIVE_RESTART);
    gl::PrimitiveRestartIndex(primitive_restart);

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

