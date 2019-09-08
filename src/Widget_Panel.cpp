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

namespace r64fx{

Widget_Panel::Widget_Panel(Widget* parent) : Widget(parent)
{
    setSize({int(Conf::Scale() * 256), int(Conf::Scale() * 256)});
}


void Widget_Panel::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(127, 127, 127));
}

}//namespace r64fx
