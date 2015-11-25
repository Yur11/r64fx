/* To be included in Panter.cpp */

struct PainterImplNormal : public PainterImpl{
    int ri = 4;
    int gi = 4;
    int bi = 4;
    int ai = 4;

    PainterImplNormal(Window* window);

    virtual ~PainterImplNormal();

    virtual void configure();

    virtual void repaint();

    virtual void clear();

};//PainterImplNormal


PainterImpl* create_normal_painter(Window* window)
{
    return new PainterImplNormal(window);
}


PainterImplNormal::PainterImplNormal(Window* window) : PainterImpl(window)
{
    window->getComponentIndices(&ri, &gi, &bi, &ai);
    clear();
}


PainterImplNormal::~PainterImplNormal()
{

}


void PainterImplNormal::configure()
{

}


void PainterImplNormal::repaint()
{
    target_image = window->image();
    root_group->paint(this);
    
}


void PainterImplNormal::clear()
{
    PainterImpl::clear();
}
