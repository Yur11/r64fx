#include "Font.h"
#include <map>
#include <iostream>

using namespace std;

namespace r64fx{

map<string, Font*> font_library;
    
Font* default_font;

void Font::initDefaultFont(Font* font)
{
    default_font = font;
}


Font* Font::defaultFont()
{
    return default_font;
}


Font* Font::loadFont(std::string fontname, std::string path)
{
    auto font = new Font(path);
    if(font->isOk())
    {
        font_library[fontname] = font;
        return font;
    }
    
    return nullptr;
}


Font* Font::find(std::string fontname)
{
    auto it = font_library.find(fontname);
    if(it == font_library.end())
    {
        return nullptr;
    }
    
    return it->second;
}
    
}//namespace r64fx