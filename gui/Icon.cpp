#include "Icon.h"
#include <map>

#include <iostream>

using namespace std;

namespace r64fx{

map<string, Icon> all_icons;

void Icon::render()
{
    if(_texture.isGood())
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _texture.bind();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
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
    else
    {
//         glColor3f(1.0, 0.0, 0.0);
//         glBegin(GL_LINE_LOOP);
//             glVertex2f(0.0, 0.0);
//             glVertex2f(size.w, 0.0);
//             glVertex2f(size.w, size.h);
//             glVertex2f(0.0, size.h);
//         glEnd();
    }
}


Icon Icon::find(std::string name, Size<float> size)
{
    auto it = all_icons.find(name);
    if(it == all_icons.end())
    {
        Texture texture("icons/" + name + ".png");
        if(texture.isGood())
        {
            Icon icon(size, texture);
            all_icons[name] = icon;
            return icon;
        }
        else
        {
            return defaultIcon();
        }
    }
    else
        return it->second;
}


Icon Icon::defaultIcon(Size<float> size)
{
    return Icon(size, Texture::badTexture());
}

    
}//namespace r64fx
