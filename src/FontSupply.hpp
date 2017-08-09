#ifndef R64FX_FONT_SUPPLY_HPP
#define R64FX_FONT_SUPPLY_HPP

#include "Font.hpp"

namespace r64fx{

Font* get_font(std::string name = "", float size = 16.0f, int dpy = 72);

void free_font(Font* font);

}//namespace r64fx

#endif//R64FX_FONT_SUPPLY_HPP
