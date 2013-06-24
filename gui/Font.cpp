#include "Font.h"
#include <map>
#include <iostream>

using namespace std;

namespace r64fx{

Font* default_font;

void Font::initDefaultFont(Font* font)
{
    default_font = font;
}


Font* Font::defaultFont()
{
    return default_font;
}
    
}//namespace r64fx