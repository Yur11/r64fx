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

#include "Widget_Panel.hpp"
#include "Painter.hpp"
#include "Conf_Scale.hpp"

#include "Widget_Knob.hpp"
#include "Widget_Slider.hpp"
#define m_knob0 ((Widget_Knob*)k0)
#define m_knob1 ((Widget_Knob*)k1)
#define m_knob2 ((Widget_Knob*)k2)
#define m_knob3 ((Widget_Knob*)k3)
#define m_slider0 ((Widget_Slider*)s0)
#define m_slider1 ((Widget_Slider*)s1)

namespace r64fx{

Widget_Panel::Widget_Panel(Widget* parent) : Widget(parent)
{
    setSize({Conf::ScaleUp(256), Conf::ScaleUp(256)});

    k0 = new Widget_Knob(Conf::ScaleUp(48), this);
    m_knob0->setPosition({Conf::ScaleUp(2), Conf::ScaleUp(12)});

    k1 = new Widget_Knob(Conf::ScaleUp(48), this);
    m_knob1->setPosition({Conf::ScaleUp(2), Conf::ScaleUp(14) + m_knob0->height()});

    k2 = new Widget_Knob(Conf::ScaleUp(48), this);
    m_knob2->setPosition({Conf::ScaleUp(2), Conf::ScaleUp(16) + m_knob0->height() + m_knob1->height()});
    m_knob2->setMinValue(0.0f);
    m_knob2->setMaxValue(+1.0f);

    k3 = new Widget_Knob(Conf::ScaleUp(48), this);
    m_knob3->setPosition({Conf::ScaleUp(2), Conf::ScaleUp(18) + m_knob0->height() + m_knob1->height() + m_knob2->height()});
    m_knob3->setMinValue(-1.0f);
    m_knob3->setMaxValue(0.0f);

    s0 = new Widget_Slider(Conf::ScaleUp(128), Conf::ScaleUp(16), Orientation::Vertical, this);
    m_slider0->setPosition({m_knob0->width() + Conf::ScaleUp(10), Conf::ScaleUp(10)});

    s1 = new Widget_Slider(Conf::ScaleUp(128), Conf::ScaleUp(16), Orientation::Vertical, this);
    m_slider1->setPosition({m_knob0->width() + m_slider0->width() + Conf::ScaleUp(20), Conf::ScaleUp(10)});
}


Widget_Panel::~Widget_Panel()
{
    
}


void Widget_Panel::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(127, 127, 127));

    childrenPaintEvent(event);
}

}//namespace r64fx
