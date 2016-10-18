#include "ProjectView.hpp"
#include "Painter.hpp"
#include "Color.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

ProjectView::ProjectView(Widget* parent) : Widget(parent)
{
    
}


ProjectView::~ProjectView()
{
    
}


void ProjectView::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(255, 255, 255, 0));
}


void ProjectView::mouseMoveEvent(MouseMoveEvent* event)
{
    auto window = Widget::rootWindow();
    if(window)
    {
        window->setCursorType(Window::CursorType::Arrow);
    }
}

}//namespace r64fx
