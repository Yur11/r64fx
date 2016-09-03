#ifndef R64FX_FILTER_VIEW_HPP
#define R64FX_FILTER_VIEW_HPP

#include "Widget.hpp"

namespace r64fx{

class FilterViewControllerIface;
class FilterViewPrivate;

class FilterView : public Widget{
    FilterViewPrivate* m = nullptr;

public:
    FilterView(FilterViewControllerIface* ctrl, Widget* parent = nullptr);

    virtual ~FilterView();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void resizeEvent(ResizeEvent* event);
};


class FilterViewControllerIface{

};

}//namespace r64fx

#endif//R64FX_FILTER_VIEW_HPP