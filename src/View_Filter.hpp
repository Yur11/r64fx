#ifndef R64FX_VIEW_FILTER_HPP
#define R64FX_VIEW_FILTER_HPP

#include "Widget.hpp"

namespace r64fx{

class View_FilterControllerIface;
class View_FilterPrivate;
class FilterClass;

class View_Filter : public Widget{
    View_FilterPrivate* m = nullptr;

public:
    View_Filter(View_FilterControllerIface* ctrl, Widget* parent = nullptr);

    virtual ~View_Filter();

    void setFilterClass(FilterClass* fc);

protected:
    virtual void resizeEvent(WidgetResizeEvent* event);
};


class View_FilterControllerIface{

};

}//namespace r64fx

#endif//R64FX_VIEW_FILTER_HPP
