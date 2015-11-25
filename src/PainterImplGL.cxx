/* To be included in Panter.cpp */


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




