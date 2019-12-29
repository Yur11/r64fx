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

#ifndef R64FX_WIDGET_PANEL_HPP
#define R64FX_WIDGET_PANEL_HPP

#include "Widget.hpp"

namespace r64fx{

class Widget_Panel : public Widget{
    void* k0 = nullptr;
    void* k1 = nullptr;
    void* k2 = nullptr;
    void* k3 = nullptr;
    void* s0 = nullptr;
    void* s1 = nullptr;

public:
    Widget_Panel(Widget* parent);

    ~Widget_Panel();

private:
    void paintEvent(WidgetPaintEvent* event);

    
};

}//namespace r64fx

#endif//R64FX_WIDGET_PANEL_HPP
