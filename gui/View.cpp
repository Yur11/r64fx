#include "View.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Window.h"
#include "Translation.h"
#include "Painter.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>

#define MAKE_SURE_WE_HAVE_A_SCENE\
    if(!_scene)\
    {\
        cerr << "View Scene is null!\n";\
        abort();\
    }
    
using namespace std;
#endif//DEBUG

namespace r64fx{
    
Icon View::split_view_vert_icon;
Icon View::split_view_hor_icon;
Icon View::close_view_icon;
    

/** Removeme */
extern Font* debug_font;

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

    
void SplittableView::findParentViewOrWindow(SplitView* &view, Window* &window)
{
    for(auto w : Window::allInstances())
    {
        if(w->view() == this)
        {
            window = w;
            return;
        }
    }
    
    for(auto w : Window::allInstances())
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
View::View(Scene* scene)
{
    setScene(scene);
    
    split_view_vert_act = new Action(
        split_view_vert_icon,
        tr("split_vertically"),  
        Message([](void* source, void* data)->void*
            {
                auto view = (View*) data;
                SplitView* parent_view = nullptr;
                Window* parent_window = nullptr;
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
        split_view_hor_icon,
        tr("split_horizontally"),     
        Message([](void* source, void* data)->void*
            {
                auto view = (View*) data;
                SplitView* parent_view = nullptr;
                Window* parent_window = nullptr;
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
        close_view_icon,
        tr("close_view"),
        Message([](void* source, void* data)->void*
            {
                auto view = (View*) data;
                SplitView* parent_view = nullptr;
                Window* parent_window = nullptr;
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
                    
                    view->deleteLater();
                    parent_view->deleteLater();
                }
                
                return nullptr;
            },
            this
        )
    );
    
    _context_menu = new Menu(debug_font);
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


void View::setScene(Scene* scene)
{
    _scene = scene;
    if(!_scene) return;
    _scene->replace_me_callback = [](View* view, Scene* old_scene, Scene* new_scene){
        view->setScene(new_scene);
    };
}
    
    
void View::resize(int left, int top, int right, int bottom) 
{
    _rect.left = left;
    _rect.top = top;
    _rect.right = right; 
    _rect.bottom = bottom;
#ifdef DEBUG
    assert(_rect.width() >= 0);
    assert(_rect.height() >= 0);
#endif//DEBUG
    _scale_center = Point<float>(left + width()*0.5, bottom + height()*0.5);
}
    
    
void View::render()
{    
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
    if(width() < 0)
    {
        cout << "View::render: width == " << width() << "\n";
    }
    
    if(height() < 0)
    {
        cout << "View::render: height == " << height() << "\n";
    }
#endif//DEBUG
    
    glScissor(x(), y(), width(), height());
    CHECK_FOR_GL_ERRORS;
    
    glEnable(GL_SCISSOR_TEST);
    CHECK_FOR_GL_ERRORS;
    
    auto p = *current_2d_projection;
    
    current_2d_projection->translate(x(), y());

    current_2d_projection->scale(scaleFactor(), scaleFactor());
    
    current_2d_projection->translate(_offset.x, _offset.y);
    
    _scene->render();
    
    *current_2d_projection = p;
    
    glDisable(GL_SCISSOR_TEST);
    CHECK_FOR_GL_ERRORS;
}


void View::mousePressEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transformEvent(event);
    event->_view = this;
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
    transformEvent(event);
    event->_view = this;
    _scene->mouseReleaseEvent(event);
}
    
    
void View::mouseMoveEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transformEvent(event);
    event->_view = this;
    _scene->mouseMoveEvent(event);
}


void View::mouseWheelEvent(MouseEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transformEvent(event);
    event->_view = this;
    _scene->mouseWheelEvent(event);
    
    if(!event->has_been_handled)
    {
        if(event->buttons() & Mouse::Button::WheelUp)
        {
            if(event->keyboardModifiers() & Keyboard::Modifier::Ctrl)
                zoomInOnce(event->originalPosition() - Point<float>(x(), y()));
            else if(event->keyboardModifiers() & Keyboard::Modifier::Shift)
                translate(10.0, 0.0);
            else
                translate(0.0, -10.0);
        }
        else if(event->buttons() & Mouse::Button::WheelDown)
        {
            if(event->keyboardModifiers() & Keyboard::Modifier::Ctrl)
                zoomOutOnce(event->originalPosition() - Point<float>(x(), y()));
            else if(event->keyboardModifiers() & Keyboard::Modifier::Shift)
                translate(-10.0, 0.0);
            else
                translate(0.0, 10.0);
        }
        
        event->has_been_handled = true;
    }
}


void View::keyPressEvent(KeyEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transformEvent(event);
    event->_view = this;
    _scene->keyPressEvent(event);
}
    
    
void View::keyReleaseEvent(KeyEvent* event)
{
#ifdef DEBUG
    MAKE_SURE_WE_HAVE_A_SCENE
#endif//DEBUG
    transformEvent(event);
    event->_view = this;
    _scene->keyReleaseEvent(event);
}


void View::transformEvent(Event* event)
{
    *event -= Point<float>(x(), y());
    *event -= _offset * scaleFactor();
    *event *= (1.0/scaleFactor());
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


void View::zoomOnce(float scale_coeff, Point<float> mouse_position)
{    
    Point<float> before_scaling = mouse_position;
    before_scaling -= _offset * scaleFactor();
    before_scaling *= (1.0/scaleFactor());
    
    setScaleFactor(scaleFactor() * scale_coeff);
    
    Point<float> after_scaling = mouse_position;
    after_scaling -= _offset * scaleFactor();
    after_scaling *= (1.0/scaleFactor());
    
    Point<float> diff = after_scaling - before_scaling;
    translate(diff);
}
    

/* ==== SplitView ======================================================================= */

/* 2 separator vertices */
Painter* sepv = nullptr;

void SplitView::init()
{
    sepv = new Painter(2);
    
    static float data[8] = {
        0.0, 0.0,
        0.0, 0.0,
        
        0.0, 0.0,
        1.0, 1.0
    };
    
    sepv->bindBuffer();
    sepv->setData(data);
    sepv->unbindBuffer();
}


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
    
//     Painter::enable();
    if(separatorIsHovered())
        if(separatorIsGrabbed())
            Painter::setColor(0.3, 0.6, 0.3, 1.0);
        else
            Painter::setColor(0.3, 0.3, 0.3, 1.0);
    else
        Painter::setColor(0.0, 0.0, 0.0, 1.0);

    Painter::useNoTexture();
    Painter::useCurrent2dProjection();

    set_separator_coords();
    sepv->bindArray();
    sepv->render(GL_LINES);
    sepv->unbindArray();
    
//     Painter::disable();
}
    
    
void SplitView::mousePressEvent(MouseEvent* event)
{   
    auto ra = viewA()->rect().to<float>();
    
    if(separatorIsHovered())
    {
        _separator_is_grabbed = true;
    }
    else
    {
        if(ra.overlaps(event->position()))
        {
            viewA()->mousePressEvent(event);
        }
        else
        {
            viewB()->mousePressEvent(event);
        }
    }
}


void SplitView::mouseReleaseEvent(MouseEvent* event)
{   
    if(separatorIsGrabbed())
    {
        _separator_is_grabbed = false;
    }
    
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
    if(separatorIsGrabbed())
    {        
        moveSeparator(event->position());
        
        /* Kluge: We only need to update  geometry of the parent parent view,
         * but since we don't have the handle here, lets just do it on all the windows.
         * Shouldn't be much of an overhead.
         */
        for(auto w : Window::allInstances())
        {
            w->updateGeometry();
        }
    }
    else if(separatorRect().overlaps(event->position()))
    {
        _separator_is_hovered = true;
    }
    else
    {
        _separator_is_hovered = false;

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
}


void SplitView::mouseWheelEvent(MouseEvent* event)
{
    auto ra = viewA()->rect().to<float>();
    
    if(ra.overlaps(event->position()))
    {
        viewA()->mouseWheelEvent(event);
    }
    else
    {
        viewB()->mouseWheelEvent(event);
    }
}


void SplitView::keyPressEvent(KeyEvent* event)
{
    auto ra = viewA()->rect().to<float>();
        
    if(ra.overlaps(event->position()))
    {
        viewA()->keyPressEvent(event);
    }
    else
    {
        viewB()->keyPressEvent(event);
    }
}

    
void SplitView::keyReleaseEvent(KeyEvent* event)
{
    auto ra = viewA()->rect().to<float>();
        
    if(ra.overlaps(event->position()))
    {
        viewA()->keyReleaseEvent(event);
    }
    else
    {
        viewB()->keyReleaseEvent(event);
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


void VerticalSplitView::set_separator_coords()
{
    float x1 = _rect.left;
    float x2 = _rect.right;
    float y =  _rect.bottom + height() * splitRatio();
    
    float pos[4] = {
        x1, y,
        x2, y
    };
    
    sepv->bindBuffer();
    sepv->setPositions(pos, 4);
    sepv->unbindBuffer();
}


Rect<float> VerticalSplitView::separatorRect()
{
    float y = _rect.bottom + height() * splitRatio();
    return Rect<float>(_rect.left, y+2, _rect.right, y-2);
}


void VerticalSplitView::moveSeparator(Point<float> p)
{
    setSplitRatio( (p.y - _rect.bottom) / _rect.height() );
}


void HorizontalSplitView::resize(int left, int top, int right, int bottom)
{
    _rect.left = left;
    _rect.top = top;
    _rect.right = right; 
    _rect.bottom = bottom; 
    
    int split_width = (width() * splitRatio());
    
    viewA()->resize(left, top, left + split_width, bottom);
    viewB()->resize(left + split_width, top, right, bottom);
}
    
    
void HorizontalSplitView::set_separator_coords()
{    
    float x = _rect.left + width() * splitRatio();
    float y1 = _rect.bottom;
    float y2 = _rect.top;
    
    float pos[4] = {
        x, y1,
        x, y2
    };
    
    sepv->bindBuffer();
    sepv->setPositions(pos, 4);
    sepv->unbindBuffer();
}


Rect<float> HorizontalSplitView::separatorRect()
{
    float x = _rect.left + width() * splitRatio();
    return Rect<float>(x-2, _rect.top, x+2, _rect.bottom);
}


void HorizontalSplitView::moveSeparator(Point<float> p)
{
    setSplitRatio( (p.x - _rect.left)  / _rect.width() );
#ifdef DEBUG
    if(splitRatio() > 1.0)
    {
        cerr << "splitRatio() == " << splitRatio() << "\n";
        cerr << "p.x: "  << p.x << "\n";
        cerr << "_rect.width() == " << _rect.width() << "\n";
        abort();
    }
#endif//DEBUG
}


}//namespace r64fx
