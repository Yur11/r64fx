#include "HoverableWidget.h"
#include "View.h"
#include "Window.h"

using namespace std;

namespace r64fx{
    
HoverableWidget* HoverableWidget::_current_hovered = nullptr;
View* HoverableWidget::_hover_view = nullptr;


bool HoverableWidget::shouldBeRenderedAsHovered()
{ 
    return isHovered() && (
        _hover_view == View::currentlyRendered() || 
        Window::mouseIsHoveringMenu() //In case a menu is open.
    ); 
}


void HoverableWidget::mouseMoveEvent(MouseEvent* event)
{
    _hover_view = event->view();
    event->hovered_widget = this;
    
    if(_current_hovered != this)
    {
        if(_current_hovered)
            _current_hovered->mouseLeaveEvent(event);
        
        mouseEnterEvent(event);
    }
                
    _current_hovered = this;
}


void HoverableWidget::mouseEnterEvent(MouseEvent* event)
{
}


void HoverableWidget::mouseLeaveEvent(MouseEvent* event)
{
}


}//namespace r64fx