#include "Icon.h" 

namespace r64fx{
    
void Icon::render()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _texture->bind();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, 0.0);
        
        glTexCoord2f(1.0, 1.0);
        glVertex2f(width(), 0.0);
        
        glTexCoord2f(1.0, 0.0);
        glVertex2f(width(), height());
        
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, height());
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}


void Icon::resizeToFitTexture()
{
    resize(_texture->width(), _texture->height());
}
    
}//namespace r64fx
