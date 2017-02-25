#include "View_Project.hpp"
#include "Painter.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

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

    Point<float> center(22.0f, 22.0f);

    Image c1(44, 44, 1);
    fill        (&c1, Color(0));
    fill_circle (&c1, Color(255), center, 21.0f);

    Image c2(44, 44, 1);
    fill        (&c2, Color(0));
    fill_circle (&c2, Color(255), center, 18.0f);
    invert      (&c2, &c2);

    combine(&c1, &c1, {0, 0}, &c2, {0, 0});

    fill(&c1, Color(0), {10, 22, 25, 22});
    fill(&c1, Color(0), {0, 37, 44, 7});

    fill(&c1, Color(0), {20, 0, 4, 8});
    fill(&c1, Color(255), {21, 1, 2, 5});

    Image c3(44, 44, 1);
    fill        (&c3, Color(0));
    fill_circle (&c3, Color(255), center, 15.0f);
    invert      (&c3, &c3);

    Image g1(44, 44, 4);
    fill_gradient_vert (&g1, 39, 4, {0, 0, g1.width(), g1.height()});
    flip_vert          (&g1);
    copy_component     (&g1, 3, {0, 0}, &c3, 0);

    Image c4(44, 44, 1);
    fill        (&c4, Color(0));
    fill_circle (&c4, Color(255), center, 14.0f);
    invert      (&c4, &c4);

    Image g2(44, 44, 4);
    fill_gradient_vert (&g2, 105, 1, {0, 0, g2.width(), g2.height()});
    flip_vert          (&g2);
    copy_component     (&g2, 3, {0, 0}, &c4, 0);


    p->blendColors({20, 20}, Colors(Color(31, 31, 31, 0)), &c1);
    p->putImage(&g1, {20, 20});
    p->putImage(&g2, {20, 20});
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
