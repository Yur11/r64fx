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

/* Main Program View */

#ifndef R64FX_MAIN_WINDOW_HPP
#define R64FX_MAIN_WINDOW_HPP

namespace r64fx{

class MainWindow{
    void* m_private = nullptr;

public:
    MainWindow();
    ~MainWindow();

    void open();
    void close();

    void onClose(void(*callback)(void* data), void* data);
};

}//namespace

#endif//R64FX_MAIN_WINDOW_HPP
