#include "Font.h"


namespace r64fx{

Font* default_font = nullptr;
    
    
void Font::init()
{
    default_font = new Font("/usr/share/fonts/TTF/LiberationMono-Regular.ttf");
    default_font->setFaceSize(16);
}


Font* Font::defaultFont()
{
    return default_font;
}


void Font::cleanup()
{
    delete default_font;
}
    
}//namespace r64fx