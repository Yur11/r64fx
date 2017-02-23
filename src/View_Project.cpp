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

    Image img(44, 44, 4);
    fill(&img, Color(0, 0, 0, 0));

    Image circle_img(44, 44, 1);
    fill(&circle_img, Color(0));
    fill_circle(&circle_img, Color(255), {21.5f, 21.5f}, 15.5f);

    Image gradient_img(44, 44, 1);
    fill_gradient_vert(&gradient_img, {0, 0, gradient_img.width(), gradient_img.height()}, 0, 5);

    Image combined_img(44, 44, 1);
    fill(&combined_img, Color(0));
    combine(&combined_img, &circle_img, {0, 0}, &gradient_img, {0, 0}, 1);

//     blend(&img, Point<int>(0, 0), Colors(Color(255, 0, 0, 0)), &combined_img);

    p->blendColors({20, 20}, Colors(Color(255, 0, 0, 0)), &combined_img);
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
