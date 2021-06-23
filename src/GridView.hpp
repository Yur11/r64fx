#ifndef R64FX_GRID_VIEW_HPP
#define R64FX_GRID_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class GridView : public Widget{
public:
    GridView(GridView* parent = nullptr);

    virtual ~GridView();

private:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void resizeEvent(WidgetResizeEvent* event);
};

}//namespace r64fx

#endif//R64FX_GRID_VIEW_HPP