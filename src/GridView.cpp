#include "GridView.hpp"
#include "Options.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"

namespace r64fx{
namespace{

Image* g_grid_tile = nullptr;
int g_instance_count = 0;

void init()
{
    if(g_instance_count > 0)
        return;

    g_grid_tile = new Image(g_options.UiScaleUp(16), g_options.UiScaleUp(16), 4);
    fill(g_grid_tile, Color(255, 0, 0, 0));
    fill({g_grid_tile, {0, 0, g_grid_tile->width(), g_grid_tile->height()}}, Color(255, 0, 0, 0));
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

    p->fillRect({0, 0, width(), height()}, Color(255, 255, 255));
    p->tileImage(g_grid_tile, {0, 0, 100, 100});
    p->fillRect({100, 100, 100, 100}, Color(0, 255, 255));

    childrenPaintEvent(event);
}


void GridView::resizeEvent(WidgetResizeEvent* event)
{

}

}//namespace r64fx
