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

//REMOVE ME
#include "Widget_Panel.hpp"
#include "Widget_Timeline.hpp"

#define m ((Widget_MainWindow*) m_private)

using namespace std;

namespace r64fx{

struct Widget_TopBar;
struct Widget_BottomBar;
struct Widget_TopPart;
struct Widget_BottomPart;
struct Widget_LeftPart;
struct Widget_RightPart;

struct Widget_MainWindow : public Widget{
    Widget_TopBar*      top_bar      = nullptr;
    Widget_BottomBar*   bottom_bar   = nullptr;
    Widget_TopPart*     top_part     = nullptr;
    Widget_BottomPart*  bottom_part  = nullptr;
    Widget_LeftPart*    left_part    = nullptr;
    Widget_RightPart*   right_part   = nullptr;

    //REMOVE ME
    Widget_Panel*       panel        = nullptr;
    Widget_Timeline*    timeline     = nullptr;

    int gap = 3;
    bool left_dock_expanded = true, right_dock_expanded = true;

    void (*on_close)(void*) = nullptr;
    void* on_close_data = nullptr;
    virtual void closeEvent() override { if(on_close) on_close(on_close_data); }

    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;

    inline bool leftPartIsOpen() const { return true; }
    inline bool rightPartIsOpen() const { return true; }
    inline bool bottomPartIsOpen() const { return true; }
};

struct Widget_TopBar : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_BottomBar : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_TopPart : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_BottomPart : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_LeftPart : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};

struct Widget_RightPart : public Widget{
    using Widget::Widget;
    virtual void paintEvent(WidgetPaintEvent* event) override;
    virtual void resizeEvent(WidgetResizeEvent* event) override;
};


MainWindow::MainWindow()
{
    /* Remove Me */
    Conf::setScale(2);

    m_private       = new Widget_MainWindow ();
    m->top_bar      = new Widget_TopBar     (m);
    m->bottom_bar   = new Widget_BottomBar  (m);
    m->top_part     = new Widget_TopPart    (m);
    m->bottom_part  = new Widget_BottomPart (m);
    m->left_part    = new Widget_LeftPart   (m);
    m->right_part   = new Widget_RightPart  (m);

    m->top_bar      ->setHeight (Conf::Scale() * 32,  false);
    m->bottom_bar   ->setHeight (Conf::Scale() * 32,  false);
    m->left_part    ->setWidth  (Conf::Scale() * 128, false);
    m->right_part   ->setWidth  (Conf::Scale() * 128, false);
    m->bottom_part  ->setHeight (Conf::Scale() * 256, false);

    //REMOVE ME
    m->panel        = new Widget_Panel(m->bottom_part);
    m->panel        ->setPosition({0, 0});
    m->timeline     = new Widget_Timeline(m->top_part);
    m->timeline     ->setPosition({5, 5});

    m->setSize({Conf::ScaleUp(800), Conf::ScaleUp(600)});
    m->openWindow();
}


MainWindow::~MainWindow()
{
    m->closeWindow();

    //REMOVE ME
    delete m->panel;
    delete m->timeline;

    delete m->top_bar;
    delete m->bottom_bar;
    delete m->top_part;
    delete m->bottom_part;
    delete m->left_part;
    delete m->right_part;
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

    childrenPaintEvent(event);
}

void Widget_BottomBar::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 0, 0));

    childrenPaintEvent(event);
}

void Widget_TopPart::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(0, 255, 255));

    childrenPaintEvent(event);
}

void Widget_BottomPart::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(0, 255, 0));

    childrenPaintEvent(event);
}

void Widget_LeftPart::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(0, 0, 255));

    childrenPaintEvent(event);
}

void Widget_RightPart::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 0, 255));

    childrenPaintEvent(event);
}


void Widget_MainWindow::resizeEvent(WidgetResizeEvent* event)
{
    auto w = event->width();
    auto h = event->height();

    top_bar->setWidth(w);
    top_bar->setPosition({0, 0});

    bottom_bar->setWidth(w);
    bottom_bar->setPosition({0, h - bottom_bar->height()});

    auto mw = w;
    auto mh = h - top_bar->height() - bottom_bar->height() - gap*2;
    auto mx = 0;
    auto my = top_bar->height() + gap;

    if(leftPartIsOpen())
    {
        left_part->setHeight(mh);
        left_part->setPosition({0, top_bar->height() + gap});
        mw -= left_part->width() + gap;
        mx = left_part->width() + gap;
    }

    if(rightPartIsOpen())
    {
        right_part->setHeight(mh);
        right_part->setPosition({w - right_part->width(), top_bar->height() + gap});
        mw -= right_part->width() + gap;
    }

    top_part    ->setPosition({mx, my});
    top_part    ->setSize({mw, mh - bottom_part->height() - gap});

    bottom_part ->setPosition({mx, my + top_part->height() + gap});
    bottom_part ->setWidth(mw);

    //REMOVE ME
    timeline    ->setSize({top_part->width() - 10, top_part->height() - 10});

    repaint();
}

void Widget_TopBar::resizeEvent(WidgetResizeEvent* event)
{

}

void Widget_BottomBar::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_TopPart::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_BottomPart::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_LeftPart::resizeEvent(WidgetResizeEvent* event)
{
    
}

void Widget_RightPart::resizeEvent(WidgetResizeEvent* event)
{
    
}

}//namespace r64fx
