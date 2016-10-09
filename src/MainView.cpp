#include "MainView.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

class TopBar;
class LeftDock;
class RightDock;
class BottomDock;
class MainPart;


struct MainViewPrivate{
    TopBar*      top_bar      = nullptr;
    LeftDock*    left_dock    = nullptr;
    RightDock*   right_dock   = nullptr;
    BottomDock*  bottom_dock  = nullptr;
    MainPart*    main_part    = nullptr;

    int gap = 2;
    
    bool left_dock_expanded   = true;
    bool right_dock_expanded  = true;
};

    
class TopBar : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    TopBar(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        setHeight(32);
    }
    
protected:
    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    }

    virtual void resizeEvent(ResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class LeftDock : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    LeftDock(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
protected:
    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    }

    virtual void resizeEvent(ResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class RightDock : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    RightDock(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
protected:
    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    }

    virtual void resizeEvent(ResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class BottomDock : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    BottomDock(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
protected:
    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    }

    virtual void resizeEvent(ResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class MainPart : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    MainPart(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
protected:
    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    }

    virtual void resizeEvent(ResizeEvent* event)
    {
    
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};

        
MainView::MainView(Widget* parent) : Widget(parent)
{
    m = new MainViewPrivate;
    
    m->top_bar      = new TopBar      (m, this);
    m->left_dock    = new LeftDock    (m, this);
    m->right_dock   = new RightDock   (m, this);
    m->bottom_dock  = new BottomDock  (m, this);
    m->main_part    = new MainPart    (m, this);
    
    m->left_dock->setWidth(300);
    m->right_dock->setWidth(100);
    m->bottom_dock->setHeight(256);
    
    setSize({800, 600});
}


MainView::~MainView()
{
    
}


void MainView::paintEvent(PaintEvent* event)
{
    Widget::paintEvent(event);
    
    auto p = event->painter();
    
    p->fillRect(
        {0, m->top_bar->height(), width(), m->gap}, 
        Color(0, 0, 0, 0)
    );
    
    if(m->left_dock->parent() == this)
    {
        p->fillRect(
            {m->left_dock->width(), m->left_dock->y(), m->gap, m->left_dock->height()},
            Color(0, 0, 0, 0)
        );
    }

    if(m->right_dock->parent() == this)
    {
        p->fillRect(
            {m->right_dock->x() - m->gap, m->right_dock->y(), m->gap, m->right_dock->height()},
            Color(0, 0, 0, 0)
        );
    }
    
    if(m->bottom_dock->parent() == this)
    {
        p->fillRect(
            {m->bottom_dock->x(), m->bottom_dock->y() - m->gap, m->bottom_dock->width(), m->gap},
            Color(0, 0, 0, 0)
        );
    }
}


void MainView::resizeEvent(ResizeEvent* event)
{
    m->top_bar->setWidth(event->width());
    m->top_bar->setPosition({0, 0});
    
    m->main_part->setY(m->top_bar->height() + m->gap);
    
    if(m->bottom_dock->parent() == this)
    {
        m->bottom_dock->setY(event->height() - m->bottom_dock->height());
        
        if(m->left_dock->parent() == this && m->left_dock_expanded)
        {
            m->bottom_dock->setX(m->left_dock->width() + m->gap);
        }
        else
        {
            m->bottom_dock->setX(0);
        }
        
        if(m->right_dock->parent() == this && m->right_dock_expanded)
        {
            m->bottom_dock->setWidth(event->width() - m->bottom_dock->x() - m->right_dock->width() - m->gap);
        }
        else
        {
            m->bottom_dock->setWidth(event->width() - m->bottom_dock->x());
        }
        
        m->main_part->setHeight(
            event->height() - m->top_bar->height() - m->bottom_dock->height() - m->gap - m->gap
        );
    }
    else
    {
        m->main_part->setHeight(
            event->height() - m->top_bar->height() - m->gap
        );
    }
    
    if(m->left_dock->parent() == this)
    {
        m->left_dock->setX(0);
        m->left_dock->setY(m->top_bar->height() + m->gap);
        
        if(m->bottom_dock->parent() == this && !m->left_dock_expanded)
        {
            m->left_dock->setHeight(event->height() - m->left_dock->y() - m->bottom_dock->height());
        }
        else
        {
            m->left_dock->setHeight(event->height() - m->left_dock->y());
        }
        
        m->main_part->setX(m->left_dock->width() + m->gap);
    }
    else
    {
        m->main_part->setX(0);
    }
    
    if(m->right_dock->parent() == this)
    {
        m->right_dock->setX(event->width() - m->right_dock->width());
        m->right_dock->setY(m->top_bar->height() + m->gap);
        
        if(m->bottom_dock->parent() == this && !m->right_dock_expanded)
        {
            m->right_dock->setHeight(event->height() - m->right_dock->y() - m->bottom_dock->height());
        }
        else
        {
            m->right_dock->setHeight(event->height() - m->right_dock->y());
        }
        
        if(m->left_dock->parent() == this)
        {
            m->main_part->setWidth(
                event->width() - m->left_dock->width() - m->right_dock->width() - m->gap - m->gap
            );
        }
        else
        {
            m->main_part->setWidth(
                event->width() - m->right_dock->width() - m->gap
            );
        }
    }
    else
    {
        if(m->left_dock->parent() == this)
        {
            m->main_part->setWidth(
                event->width() - m->left_dock->width() - m->gap
            );
        }
        else
        {
            m->main_part->setWidth(event->width());
        }
    }
}


void MainView::mouseMoveEvent(MouseMoveEvent* event)
{
//     auto window = Widget::rootWindow();
//     if(window)
//     {
//         if(event->y() < m->main_part->height())
//         {
//             window->setCursorType(Window::CursorType::ResizeNS);
//         }
//         else if(event->y() < m->bottom_dock->y())
//         {
//             if(event->y() >= (m->main_part->y() + m->main_part->height()))
//             {
//                 window->setCursorType(Window::CursorType::ResizeNS);
//             }
//             else
//             {
//                 if(event->x() < m->left_dock->width() || event->x() >= (m->main_part->x() + m->main_part->width()))
//                 {
//                     window->setCursorType(Window::CursorType::ResizeWE);
//                 }
//             }
//         }
//     }
}
    
}//namespace r64fx
