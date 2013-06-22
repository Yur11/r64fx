#include "Font.h"
#include "data_paths.h"
#include <map>
#include <iostream>

using namespace std;

namespace r64fx{

    
map<string, Font*> all_fonts;
Font* default_font = nullptr;

    
void Font::init()
{
    string default_font_name = "Xolonium-Regular.otf";
    default_font = find(default_font_name);
    if(default_font == nullptr)
    {
        throw Font::Error("Failed to find default font \"" + default_font_name + "\"!");
    }
}


Font* Font::defaultFont()
{
    return default_font;
}


Font* Font::find(string name)
{
    auto &font = all_fonts[name];
    if(font != nullptr) return font;
    
    for(auto &path : r64fx::data_paths())
    {
        string str = path + "fonts/" + name;
        Font f(str);
        if(f.isOk())
        {
            font = new Font(str);
            return font;
        }
    }
    
    return nullptr;
}


void Font::cleanup()
{
    for(auto pair : all_fonts)
    {
        if(pair.second != nullptr)
            delete pair.second;
    }
}
    
}//namespace r64fx