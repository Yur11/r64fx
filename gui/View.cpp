#include "View.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

#ifdef DEBUG
#include <iostream>

#define MAKE_SURE_WE_HAVE_A_SCENE\
    if(!_scene)\
    {\
        cerr << "View Scene is null!\n";\
        abort();\
    }
    
using namespace std;
#endif//DEBUG

namespace r64fx{
    
    
/* ==== View ================================================================================ */
View::View(Scene* scene) : _scene(scene)
{
}
    
    
void View::resize(int left, int top, int right, int bottom) 
{
    _rect.left = left;
    _rect.top = top;
    _rect.right = right; 
    _rect.bottom = bottom; 
}
    
    
void View::render()
{    
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    
    glScissor(x(), y(), width(), height());
    
    glPushMatrix();
    glTranslatef(x(), y(), 0.0);
    _scene->render();
    glPopMatrix();
}


void View::transform_mouse_event(MouseEvent* event)
{
    *event *= Point<float>(_scale_factor, _scale_factor) - _offset;
}


void View::mousePressEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transform_mouse_event(event);
    _scene->mousePressEvent(event);    
}
    
    
void View::mouseReleaseEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transform_mouse_event(event);
    _scene->mouseReleaseEvent(event);
}
    
    
void View::mouseMoveEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transform_mouse_event(event);
    _scene->mouseMoveEvent(event);
}


VerticalSplitView* View::splitVertically(float ratio)
{
    auto a = new View;
    *a = *this;
    
    auto b = new View;
    *b = *this;
    
    VerticalSplitView* vsv = new VerticalSplitView;
    vsv->setViewA(a);
    vsv->setViewB(b);
    vsv->setSplitRatio(ratio);
    
    return vsv;
}


HorizontalSplitView* View::splitHorizontally(float ratio)
{
    auto a = new View;
    *a = *this;
    
    auto b = new View;
    *b = *this;
    
    HorizontalSplitView* hsv = new HorizontalSplitView;
    hsv->setViewA(a);
    hsv->setViewB(b);
    hsv->setSplitRatio(ratio);
    
    return hsv;
}


/* ==== SplitView ======================================================================= */
void SplitView::render()
{    
    glEnable(GL_SCISSOR_TEST);
    viewA()->render();
    viewB()->render();
    glDisable(GL_SCISSOR_TEST);
    
    render_separator();
}
    
void SplitView::mousePressEvent(MouseEvent* event)
{   
//     event->view = this;
    
    auto ra = viewA()->rect().to<float>();
    auto rb = viewB()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        *event -= ra.position();
        viewA()->mousePressEvent(event);
    }
    else
    {
        *event -= rb.position();
        viewB()->mousePressEvent(event);
    }
}

void SplitView::mouseReleaseEvent(MouseEvent* event)
{
//     event->view = this;
    
    auto ra = viewA()->rect().to<float>();
    auto rb = viewB()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        *event -= ra.position();
        viewA()->mouseReleaseEvent(event);
    }
    else
    {
        *event -= rb.position();
        viewB()->mouseReleaseEvent(event);
    }
}


void SplitView::mouseMoveEvent(MouseEvent* event)
{
//     event->view = this;
    
    auto ra = viewA()->rect().to<float>();
    auto rb = viewB()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        *event -= ra.position();
        viewA()->mouseMoveEvent(event);
    }
    else
    {
        *event -= rb.position();
        viewB()->mouseMoveEvent(event);
    }
}


void VerticalSplitView::resize(int left, int top, int right, int bottom)
{
    _rect.left = left;
    _rect.top = top;
    _rect.right = right; 
    _rect.bottom = bottom; 
    
    int split_height = (height() * splitRatio());
    
    viewB()->resize(left, top, right, bottom + split_height);
    
    viewA()->resize(left, top - (height() - split_height), right, bottom);
}


void VerticalSplitView::render_separator()
{
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex2f(_rect.left, height() * splitRatio());
        glVertex2f(_rect.right, height() * splitRatio());
    glEnd();
}


void HorizontalSplitView::resize(int left, int top, int right, int bottom)
{
    _rect.left = left;
    _rect.top = top;
    _rect.right = right; 
    _rect.bottom = bottom; 

    int split_width = (width() * splitRatio());
    
    viewA()->resize(left, top, right - split_width, bottom);
    
    viewB()->resize(left + (width() - split_width), top, right, bottom);
}
    
    
void HorizontalSplitView::render_separator()
{
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex2f(width() * splitRatio(), _rect.top);
        glVertex2f(width() * splitRatio(), _rect.bottom);
    glEnd();
}


}//namespace r64fx
