#include "Menu.h"
#include "MouseEvent.h"
#include "Window.h"
#include "Painter.h"

#include <iostream>

using namespace std;


namespace r64fx{
   
    
 
template<typename T> T max(T a, T b)
{
    return a > b ? a : b;
}
 

    
Menu::Menu(Font* font, Widget* parent)
: LinearContainer(parent)
,_font(font)
, p(4)
{
    float tex_coords[8] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    p.bindBuffer();
    p.setTexCoords(tex_coords, 8);
    p.unbindBuffer();
}


void Menu::appendAction(Action* act)
{
    appendWidget(new ActionWidget(act, _font));
}


void Menu::update()
{
    const auto &r = absoluteRect();
        
    float pos[8] = {
        0.0, 0.0,
        r.width(), 0.0,
        0.0, r.height(),
        r.width(), r.height()
    };
    
    p.bindBuffer();
    p.setPositions(pos, 8);
    p.unbindBuffer();
}


void Menu::render()
{        
//     Painter::useCurrent2dProjection();
    Painter::useNoTexture();
    Painter::setColor(0.0, 0.0, 0.0, 1.0);
    
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();        
}

    
}//namespace r64fx