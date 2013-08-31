#include "View.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowBase.h"

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
    
/** Returns nullptr if view is not found down the tree or the parent SplitView of the target view. */
SplitView* parent_for_view_down_the_tree(SplittableView* root, SplittableView* view)
{
    if(dynamic_cast<SplitView*>(root))
    {
        auto split_view = (SplitView*)root;
        if(split_view->viewA() == view)
        {
            return split_view;
        }
        
        if(split_view->viewB() == view)
        {
            return split_view;
        }
        
        SplitView* parent = parent_for_view_down_the_tree(split_view->viewA(), view);
        if(!parent)
        {
            parent = parent_for_view_down_the_tree(split_view->viewB(), view);
        }
        
        return parent;
    }
    else
    {
    }
    
    return nullptr;
}

    
void SplittableView::findParentViewOrWindow(SplitView* &view, WindowBase* &window)
{
    for(auto w : WindowBase::allInstances())
    {
        if(w->view() == this)
        {
            window = w;
            return;
        }
    }
    
    for(auto w : WindowBase::allInstances())
    {
        SplitView* split_view = parent_for_view_down_the_tree(w->view(), this);
        if(split_view)
        {
            window = w;
            view = split_view;
        }
    }
}


    
/* ==== View ================================================================================ */
View::View(Scene* scene) : _scene(scene)
{
    split_view_vert_act = new Action(
        Icon::find("split_vertically", 24, 24),
        "Split Vertically",  
        Message([](void* source, void* data)->void*
            {
                auto view = (View*) data;
                SplitView* parent_view = nullptr;
                WindowBase* parent_window = nullptr;
                view->findParentViewOrWindow(parent_view, parent_window);
                if(parent_view)
                {
                    parent_view->replaceSubView(view, view->splitVertically(0.5));
                }
                else if(parent_window)
                {
                    parent_window->setView(view->splitVertically(0.5));
                }
                parent_window->updateGeometry();
                return nullptr;
            },
            this
        )
    );
    
    split_view_hor_act = new Action(
        Icon::find("split_horizontally", 24, 24),
        "Split Horizontally",     
        Message([](void* source, void* data)->void*
            {
                auto view = (View*) data;
                SplitView* parent_view = nullptr;
                WindowBase* parent_window = nullptr;
                view->findParentViewOrWindow(parent_view, parent_window);
                if(parent_view)
                {
                    parent_view->replaceSubView(view, view->splitHorizontally(0.5));
                }
                else if(parent_window)
                {
                    parent_window->setView(view->splitHorizontally(0.5));
                }
                parent_window->updateGeometry();
                return nullptr;
            },
            this
        )
    );
    
    close_view_act = new Action(
        Icon::find("close_view", 24, 24),
        "Close View",
        Message([](void* source, void* data)->void*
            {
                auto view = (View*) data;
                SplitView* parent_view = nullptr;
                WindowBase* parent_window = nullptr;
                view->findParentViewOrWindow(parent_view, parent_window);
                if(parent_view)
                {
                    SplitView* parent_view_of_parent = nullptr;
                    parent_view->findParentViewOrWindow(parent_view_of_parent, parent_window);
                    SplittableView* view_to_keep = (
                        parent_view->viewB() == view ?
                        parent_view->viewA() : parent_view->viewB()
                    );
                    if(parent_view_of_parent)
                    {
                        parent_view_of_parent->replaceSubView(parent_view, view_to_keep);
                    }
                    else
                    {
                        parent_window->setView(view_to_keep);
                    }
                    
                    parent_window->updateGeometry();
                    
                    /* Is it ok to delete these here? */
                    delete view;
                    delete parent_view;
                }
                
                return nullptr;
            },
            this
        )
    );
    
    _context_menu = new Menu;
    _context_menu->appendAction(split_view_vert_act);
    _context_menu->appendAction(split_view_hor_act);
    _context_menu->appendAction(close_view_act);
    _context_menu->setPadding(5);
    _context_menu->update();
}


View::~View()
{
    delete _context_menu;
    delete split_view_vert_act;
    delete split_view_hor_act;
    delete close_view_act;
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
    
    glEnable(GL_SCISSOR_TEST);
    glPushMatrix();
    glTranslatef(x(), y(), 0.0);
    _scene->render();
    glPopMatrix();
    glDisable(GL_SCISSOR_TEST);
    
}


void View::transform_mouse_event(MouseEvent* event)
{
    *event -= rect().to<float>().position();
    *event *= Point<float>(_scale_factor, _scale_factor) - _offset;
}


void View::mousePressEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transform_mouse_event(event);
    _scene->mousePressEvent(event);
    
    if(!event->has_been_handled && event->buttons() & Mouse::Button::Right)
    {
        event->originWindow()->showOverlayMenu(event->original_x(), event->original_y(), _context_menu);
        event->has_been_handled = true;
    }
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
    auto a = this;
    
    auto b = new View;
    b->setScene(a->scene());
    b->setOffset(a->offset());
    b->setScaleFactor(a->scaleFactor());
    
    VerticalSplitView* vsv = new VerticalSplitView;
    vsv->setViewA(a);
    vsv->setViewB(b);
    vsv->setSplitRatio(ratio);
    vsv->resize(vsv->rect().left, vsv->rect().top, vsv->rect().right, vsv->rect().bottom);
    
    return vsv;
}


HorizontalSplitView* View::splitHorizontally(float ratio)
{
    auto a = this;
    
    auto b = new View;
    b->setScene(a->scene());
    b->setOffset(a->offset());
    b->setScaleFactor(a->scaleFactor());
    
    HorizontalSplitView* hsv = new HorizontalSplitView;
    hsv->setViewA(a);
    hsv->setViewB(b);
    hsv->setSplitRatio(ratio);
    hsv->resize(hsv->rect().left, hsv->rect().top, hsv->rect().right, hsv->rect().bottom);
    
    return hsv;
}


/* ==== SplitView ======================================================================= */
void SplitView::replaceSubView(SplittableView* old_view, SplittableView* new_view)
{
    if(old_view == viewA())
        setViewA(new_view);
    
    if(old_view == viewB())
        setViewB(new_view);
}


void SplitView::render()
{
    viewA()->render();
    viewB()->render();
    
    render_separator();
}
    
void SplitView::mousePressEvent(MouseEvent* event)
{   
    auto ra = viewA()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        viewA()->mousePressEvent(event);
    }
    else
    {
        viewB()->mousePressEvent(event);
    }
}

void SplitView::mouseReleaseEvent(MouseEvent* event)
{   
    auto ra = viewA()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        viewA()->mouseReleaseEvent(event);
    }
    else
    {
        viewB()->mouseReleaseEvent(event);
    }
}


void SplitView::mouseMoveEvent(MouseEvent* event)
{
    auto ra = viewA()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        viewA()->mouseMoveEvent(event);
    }
    else
    {
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
    
    viewA()->resize(left, top - (height() - split_height), right, bottom);
    viewB()->resize(left, top, right, bottom + split_height);    
}


void VerticalSplitView::render_separator()
{
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex2f(_rect.left, _rect.bottom + height() * splitRatio());
        glVertex2f(_rect.right, _rect.bottom + height() * splitRatio());
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
        glVertex2f(_rect.left + width() * splitRatio(), _rect.top);
        glVertex2f(_rect.left + width() * splitRatio(), _rect.bottom);
    glEnd();
}


}//namespace r64fx
