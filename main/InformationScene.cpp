#include "InformationScene.h"
#include <iostream>

using namespace std;

namespace r64fx{

InformationScene::InformationScene()
{
    scale_step = 1.0;
    scroll_x_step = 5.0;
    scroll_y_step = 0.0;
    
    panel = new Panel;
    appendWidget(panel);
    panel->setPosition(0.0, 0.0);
    panel->resize(2000, 24);
    panel->setPaddingTop(4.0);
    panel->update();
}


InformationScene::Panel::Panel()
: p(4)
{
    float data[8] = {        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p.bindBuffer();
    p.setTexCoords(data, 8);
    p.unbindBuffer();
}

void InformationScene::Panel::update()
{
    HorizontalContainer::update();
    setWidth(1000);
    
    float data[8] = {
        0.0, 0.0,
        width(), 0.0,
        0.0, height(),
        width(), height(),
    };
    
    p.bindBuffer();
    p.setPositions(data, 8);
    p.unbindBuffer();
}

void InformationScene::Panel::render()
{    
    p.setColor(0.0, 0.0, 0.0, 1.0);
    p.useNoTexture();
    p.useCurrent2dProjection();
    p.bindArray();
    p.render(GL_TRIANGLE_STRIP);
    p.unbindArray();
    
    render_children();
}
    
}//namespace r64fx