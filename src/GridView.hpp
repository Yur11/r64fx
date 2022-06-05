#ifndef R64FX_GRID_VIEW_HPP
#define R64FX_GRID_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class GridView : public Widget{
    long m_offset_x = 0, m_offset_y = 0;

public:
    GridView(GridView* parent = nullptr);

    virtual ~GridView();

private:
    virtual void paintEvent(WidgetPaintEvent* event) override final;

    virtual void resizeEvent(WidgetResizeEvent* event) override final;

    virtual void mouseMoveEvent(MouseMoveEvent* event) override final;
};

}//namespace r64fx

#endif//R64FX_GRID_VIEW_HPP