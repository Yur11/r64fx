#include "GridView.hpp"
#include "Options.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"

#include <iostream>

using namespace std;

namespace r64fx{
namespace{

Image* g_grid_tile = nullptr;
int g_instance_count = 0;

int g_tile_size = 16;

void init()
{
    if(g_instance_count > 0)
        return;

    g_grid_tile = new Image(g_options.UiScaleUp(g_tile_size), g_options.UiScaleUp(g_tile_size), 4);
    fill({g_grid_tile, {0, 0, g_grid_tile->width(), g_grid_tile->height()}}, Color(255, 255, 0, 0));
    fill({g_grid_tile, {
        int(g_options.ui_scale),
        int(g_options.ui_scale),
        g_grid_tile->width() - int(g_options.ui_scale),
        g_grid_tile->height() - int(g_options.ui_scale)
    }}, Color(0, 255, 0, 0));

    g_instance_count++;
}

void cleanup()
{
    g_instance_count--;

    if(g_instance_count > 0)
        return;

    delete g_grid_tile;
}

}//namespace


GridView::GridView(GridView* parent) : Widget(parent)
{
    init();
    setSize({g_options.UiScaleUp(800), g_options.UiScaleUp(600)});
}


GridView::~GridView()
{
    cleanup();
}


void GridView::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->tileImage(g_grid_tile, {0, 0, width(), height()}, {int(m_offset_x % g_options.UiScaleUp(g_tile_size)), int(m_offset_y % g_options.UiScaleUp(g_tile_size))});

    childrenPaintEvent(event);
}


void GridView::resizeEvent(WidgetResizeEvent* event)
{

}


void GridView::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        cout << event->x() << ", " << event->y() << "\n";
        m_offset_x += event->dx();
        m_offset_y += event->dy();
    }
    repaint();
}

}//namespace r64fx
