/* To be included in Panter.cpp */

namespace{
    int PainterImplGL_count = 0;

    bool gl_stuff_is_good = false;

    Shader_rgba* g_Shader_rgba = nullptr;
}


class PaintLayer;

struct PainterImplGL : public PainterImpl{
    vector<PaintLayer*> layers;

    PainterImplGL(Window* window);

    virtual ~PainterImplGL();

    virtual void configure();

    void traverseCommands(LinkedList<PaintCommand>::Iterator begin, LinkedList<PaintCommand>::Iterator end);

    void dispatchCommand(PaintCommand_FillRect* pc);

    void dispatchCommand(PaintCommand_PutImage* pc);

    virtual void repaint();

    virtual void clear();

    static void initGLStuffIfNeeded();

    static void cleanupGLStuff();

};//PainterImplGL


PainterImpl* create_gl_painter(Window* window)
{
    return new PainterImplGL(window);
}


/** @brief A bunch of PaintCommand instances of the same type that can be executed together.

    Commands grouped in a layer must be of the same type. i.e they must use the same shader
    and the same uniform values.
    This type of grouping allows us to reduce the number of gl calls.
 */
struct PaintLayer{
    virtual PaintCommand::Type type() = 0;

    virtual ~PaintLayer() {}

    /** @brief Prepare for gl drawing. Upload vertex or texture data etc.  */
    virtual void configure() = 0;

    /** @brief Use shader, bind array and issue gl draw commands. */
    virtual void draw(PainterImplGL* painter) = 0;
};


struct PaintLayer_FillRect : public PaintLayer{
    VertexArray_rgba* va = nullptr;

    vector<PaintCommand*> commands;

    inline void add(PaintCommand_FillRect* cmd) { commands.push_back(cmd); }

    virtual PaintCommand::Type type() { return PaintCommand::Type::FillRect; }

    virtual ~PaintLayer_FillRect() {}

    virtual void configure()
    {
        float* positions = new float[commands.size()*8];
        unsigned char* colors = new unsigned char[commands.size()*16];

        for(int i=0; i<(int)commands.size(); i++)
        {
            auto c = (PaintCommand_FillRect*) commands[i];
            const Rect<int>             &rect  = c->rect;
            const Color<unsigned char>  &color = c->color;

            positions[i*8 + 0] = rect.x();
            positions[i*8 + 1] = rect.y();
            positions[i*8 + 2] = rect.x() + rect.width();
            positions[i*8 + 3] = rect.y();
            positions[i*8 + 4] = rect.x() + rect.width();
            positions[i*8 + 5] = rect.y() + rect.height();
            positions[i*8 + 6] = rect.x();
            positions[i*8 + 7] = rect.y() + rect.height();

            for(int c=0; c<16; c++)
            {
                colors[i*16 + c] = color[c & 3];
            }
        }

        va = new VertexArray_rgba(g_Shader_rgba, commands.size()*4);
        va->loadPositions(positions, 0, commands.size()*4);
        va->loadColors(colors, 0, commands.size()*4);

        delete positions;
        delete colors;
    }

    virtual void draw(PainterImplGL* painter)
    {
        auto window = painter->window;

        g_Shader_rgba->use();
        g_Shader_rgba->setScaleAndShift(
             2.0f/float(window->width()),
            -2.0f/float(window->height()),
            -1.0f,
             1.0f
        );
        va->bind();

        for(int i=0; i<(int)commands.size(); i++)
        {
            gl::DrawArrays(GL_TRIANGLE_FAN, i*4, 4);
        }
    }
};


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


void PainterImplGL::configure()
{
    /* Group commands into layers. */
    if(root_group)
    {
        traverseCommands(root_group->commands.begin(), root_group->commands.end());
    }

    for(auto layer : layers)
    {
        layer->configure();
    }
}


void PainterImplGL::traverseCommands(LinkedList<PaintCommand>::Iterator begin, LinkedList<PaintCommand>::Iterator end)
{
    for(auto it=begin; it!=end; ++it)
    {
        auto pc = *it;
        switch(pc->type())
        {
            case PaintCommand::Type::Group:
            {
                break;
            }

            case PaintCommand::Type::FillRect:
            {
                dispatchCommand((PaintCommand_FillRect*)pc);
                break;
            }

            case PaintCommand::Type::PutImage:
            {
                dispatchCommand((PaintCommand_PutImage*)pc);
                break;
            }
        }
    }
    cout << "\n";
}


void PainterImplGL::dispatchCommand(PaintCommand_FillRect* pc)
{
    cout << "dc: FillRect\n";
    PaintLayer_FillRect* layer = nullptr;
    if(layers.empty() || layers.back()->type() != PaintCommand::Type::FillRect)
    {
        cout << "new FillRect layer\n";
        layer = new PaintLayer_FillRect;
        layers.push_back(layer);
    }
    else
    {
        layer = (PaintLayer_FillRect*)layers.back();
    }

    layer->add(pc);
}


void PainterImplGL::dispatchCommand(PaintCommand_PutImage* pc)
{
    cout << "dc: PutImage\n";
}


void PainterImplGL::repaint()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::Clear(GL_COLOR_BUFFER_BIT);
    for(auto layer : layers)
    {
        layer->draw(this);
    }
    window->repaint();
}


void PainterImplGL::clear()
{
    for(auto layer : layers)
    {
        delete layer;
    }
    layers.clear();
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

