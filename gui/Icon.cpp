#include "Icon.h"
#include <map>


using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

Icon default_icon;
    
void Icon::render()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _texture->bind();//texture
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, 0.0);
        
        glTexCoord2f(1.0, 1.0);
        glVertex2f(size.w, 0.0);
        
        glTexCoord2f(1.0, 0.0);
        glVertex2f(size.w, size.h);
        
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, size.h);
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}


Icon Icon::find(std::string name)
{
    auto it = all_icons.find(name);
    if(it == all_icons.end())
        return defaultIcon();
    else
        return it->second;
}


Icon Icon::defaultIcon()
{

}

    
}//namespace r64fx
