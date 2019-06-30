/* This file is part of r64fx.
 *
 * r64fx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * r64fx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with r64fx. If not, see <https://www.gnu.org/licenses/>.
 */

/* Main Program View */

#include <iostream>

#include "MainWindow.hpp"
#include "Widget.hpp"
#include "Painter.hpp"
#include "Conf_Scale.hpp"

#define m ((Widget_MainWindow*) m_private)

using namespace std;

namespace r64fx{

struct Widget_TopBar;
struct Widget_MainPart;
struct Widget_MiddleBar;
struct Widget_BottomDock;
struct Widget_LeftDock;
struct Widget_RightDock;

struct Widget_MainWindow : public Widget{
    Widget_TopBar*      top_bar      = nullptr;
    Widget_MainPart*    main_part    = nullptr;
    Widget_MiddleBar*   middle_bar   = nullptr;
    Widget_BottomDock*  bottom_dock  = nullptr;
    Widget_LeftDock*    left_dock    = nullptr;
    Widget_RightDock*   right_dock   = nullptr;

    int gap = 1;
    bool left_dock_expanded = true, right_dock_expanded = true;

    void (*on_close)(void*) = nullptr;
    void* on_close_data = nullptr;
    virtual void closeEvent() override { if(on_close) on_close(on_close_data); }

    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_TopBar : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_MainPart : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_MiddleBar : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_BottomDock : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_LeftDock : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_RightDock : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};


MainWindow::MainWindow()
{
    m_private       = new Widget_MainWindow ();
    m->top_bar      = new Widget_TopBar     (m);
    m->main_part    = new Widget_MainPart   (m);
    m->middle_bar   = new Widget_MiddleBar  (m);
    m->bottom_dock  = new Widget_BottomDock (m);
    m->left_dock    = new Widget_LeftDock   (m);
    m->right_dock   = new Widget_RightDock  (m);

    m->top_bar      ->setHeight (Conf::Scale() * 32,  false);
    m->middle_bar   ->setHeight (Conf::Scale() * 32,  false);
    m->bottom_dock  ->setHeight (Conf::Scale() * 256, false);
    m->left_dock    ->setWidth  (Conf::Scale() * 256, false);
    m->right_dock   ->setWidth  (Conf::Scale() * 128, false);

    m->setSize({Conf::Scale() * 800, Conf::Scale() * 600});
    m->openWindow();
}


MainWindow::~MainWindow()
{
    m->closeWindow();

    delete m->top_bar;
    delete m->main_part;
    delete m->middle_bar;
    delete m->bottom_dock;
    delete m->left_dock;
    delete m->right_dock;
    delete m;
}


void MainWindow::open()  { m->openWindow(); }
void MainWindow::close() { m->closeWindow(); }


void MainWindow::onClose(void(*callback)(void* data), void* data)
{
    m->on_close = callback;
    m->on_close_data = data;
}


void Widget_MainWindow::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 255, 255));

    childrenPaintEvent(event);
}

void Widget_TopBar::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 0, 0));
}

void Widget_MainPart::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(0, 255, 255));
}

void Widget_MiddleBar::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 0, 0));
}

void Widget_BottomDock::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(0, 255, 0));
}

void Widget_LeftDock::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(0, 0, 255));
}

void Widget_RightDock::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 0, 255));
}


void Widget_MainWindow::resizeEvent(WidgetResizeEvent* event)
{
    top_bar->setWidth(event->width());
    top_bar->setPosition({0, 0});

    main_part->setY(top_bar->height() + gap);

    if(bottom_dock->parent() == this)
    {
        bottom_dock->setY(event->height() - bottom_dock->height());

        if(left_dock->parent() == this && left_dock_expanded)
            bottom_dock->setX(left_dock->width() + gap);
        else
            bottom_dock->setX(0);

        if(right_dock->parent() == this && right_dock_expanded)
            bottom_dock->setWidth(event->width() - bottom_dock->x() - right_dock->width() - gap);
        else
            bottom_dock->setWidth(event->width() - bottom_dock->x());

        main_part->setHeight(event->height() - top_bar->height() - middle_bar->height() - bottom_dock->height() - gap*3);
    }
    else
    {
        main_part->setHeight(event->height() - top_bar->height() - middle_bar->height() - gap*2);
    }

    if(left_dock->parent() == this)
    {
        left_dock->setX(0);
        left_dock->setY(top_bar->height() + gap);

        if(bottom_dock->parent() == this && !left_dock_expanded)
            left_dock->setHeight(event->height() - left_dock->y() - bottom_dock->height());
        else
            left_dock->setHeight(event->height() - left_dock->y());

        main_part->setX(left_dock->width() + gap);
    }
    else
    {
        main_part->setX(0);
    }

    if(right_dock->parent() == this)
    {
        right_dock->setX(event->width() - right_dock->width());
        right_dock->setY(top_bar->height() + gap);

        if(bottom_dock->parent() == this && !right_dock_expanded)
            right_dock->setHeight(event->height() - right_dock->y() - bottom_dock->height());
        else
            right_dock->setHeight(event->height() - right_dock->y());

        if(left_dock->parent() == this)
            main_part->setWidth(event->width() - left_dock->width() - right_dock->width() - gap - gap);
        else
            main_part->setWidth(event->width() - right_dock->width() - gap);
    }
    else
    {
        if(left_dock->parent() == this)
            main_part->setWidth(event->width() - left_dock->width() - gap);
        else
            main_part->setWidth(event->width());
    }

    middle_bar->setWidth(main_part->width());
    middle_bar->setX(main_part->x());
    middle_bar->setY(main_part->y() + main_part->height() + gap);

    repaint();
}

void Widget_TopBar::resizeEvent(WidgetResizeEvent* event)
{

}

void Widget_MainPart::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_MiddleBar::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_BottomDock::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_LeftDock::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_RightDock::resizeEvent(WidgetResizeEvent* event)
{
    
}

}//namespace r64fx
