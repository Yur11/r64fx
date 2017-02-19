#ifndef R64FX_LINEAR_CONTAINER_HPP
#define R64FX_LINEAR_CONTAINER_HPP

#include "Widget.hpp"
#include "Padding.hpp"

namespace r64fx{

class Widget_Container : public Widget, public Padding<int>{
    int m_spacing = 0;

public:
    Widget_Container(Widget* parent = nullptr);

    inline void setSpacing(int spacing) { m_spacing = spacing; }

    inline int spacing() const { return m_spacing; }

    void alignVertically();

    void alignHorizontally();
};

}//namespace r64fx

#endif//R64FX_LINEAR_CONTAINER_HPP