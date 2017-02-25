#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

void draw_solid_horse_shoe(Image* dst, Point<int> dstpos, int size, bool middle_notch = true)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(dst->componentCount() == 1);
#endif//R64FX_DEBUG
    int hs = size / 2;

    Point<float> center(dstpos.x() + hs - 0.5f, dstpos.y() + hs - 0.5f);
    fill(dst, Color(255), {dstpos.x(), dstpos.y(), size, size});
    fill_circle(dst, Color(0),   center, hs - 1 + 0.5f);
    fill_circle(dst, Color(255), center, hs - 4 + 0.5f);
    fill_bottom_triangle(dst, Color(255), dstpos, size);

    if(middle_notch)
    {
        fill(dst, Color(255), {dstpos.x() + hs - 2, dstpos.y(), 4, 5});
        fill(dst, Color(0), {dstpos.x() + hs - 1, dstpos.y(), 2, 5});
    }
}


View_Project::View_Project(Widget* parent) : Widget(parent)
{
    
}


View_Project::~View_Project()
{
    
}


void View_Project::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    for(int i=0; i<12; i++)
    {
        int size = 32 + i * 2;
        Image horse_shoe(size, size, 1);
        draw_solid_horse_shoe(&horse_shoe, {0, 0}, size);

        Image img(size, size, 4);
        fill(&img, Color(255, 255, 255, 0));
        copy_component(&img, 3, {0, 0}, &horse_shoe, 0);

        p->fillRect({20 + (i * size + 20), 20, size, size}, Color(127, 0, 0, 0));
        p->putImage(&img, {20 + (i * size + 20), 20});
    }
}


void View_Project::mouseMoveEvent(MouseMoveEvent* event)
{
    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}

}//namespace r64fx
