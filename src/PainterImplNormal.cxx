/* To be included in Panter.cpp */

PainterImplNormal::PainterImplNormal(Window* window) : PainterImpl(window)
{
    window->getComponentIndices(&ri, &gi, &bi, &ai);
    clear();
}


PainterImplNormal::~PainterImplNormal()
{

}


void PainterImplNormal::repaint()
{
    cout << "PainterImplNormal::repaint()\n";

    for(auto c : commands)
    {
        c->paint(this);
    }
    window->repaint();
}


void PainterImplNormal::clear()
{
    PainterImpl::clear();
}


void PaintCommand_FillRect::paint(PainterImplNormal* p)
{
    cout << "PaintCommand_FillRect::paint()\n";

    unsigned char px[5];
    px[p->ri] = color.red() * 255;
    px[p->gi] = color.green() * 255;
    px[p->bi] = color.blue() * 255;
    px[p->ai] = color.alpha();
    auto img = p->window->image();

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            img->setPixel(x + rect.x(), y + rect.y(), px);
        }
    }
}


void PaintCommand_PutImage::paint(PainterImplNormal* p)
{
    auto dst = p->window->image();
    auto src = img;

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            for(int c=0; c<img->channelCount(); c++)
            {
                dst->pixel(x + rect.x(), y + rect.y())[c] = src->pixel(x, y)[c];
            }
        }
    }
}


void PaintCommand_PutDensePlotHorizontal::paint(PainterImplNormal* p)
{
    auto img = p->window->image();

    float scale = 1.0f / orig_rect.height();

    for(int x=0; x<rect.width(); x++)
    {
        float min = data[x*2];
        float max = data[x*2 + 1];
        for(int y=0; y<rect.height(); y++)
        {
            float val = y*scale;
            unsigned char px[4];
            if(val > min && val < max)
            {
                px[0] = 0;
                px[1] = 0;
                px[2] = 0;
                px[3] = 0;
            }
            else
            {
                px[0] = 255;
                px[1] = 255;
                px[2] = 255;
                px[3] = 0;
            }

            img->setPixel(x + rect.x(), y + rect.y(), px);
        }
    }
}


void PaintCommand_PutDensePlotVertical::paint(PainterImplNormal* p)
{
    auto img = p->window->image();

    float scale = 1.0f / orig_rect.width();

    for(int y=0; y<rect.height(); y++)
    {
        float min = data[y*2];
        float max = data[y*2 + 1];
        for(int x=0; x<rect.width(); x++)
        {
            float val = x*scale;
            unsigned char px[4];
            if(val > min && val < max)
            {
                px[0] = 0;
                px[1] = 0;
                px[2] = 0;
                px[3] = 0;
            }
            else
            {
                px[0] = 255;
                px[1] = 255;
                px[2] = 255;
                px[3] = 0;
            }

            img->setPixel(x + rect.x(), y + rect.y(), px);
        }
    }
}