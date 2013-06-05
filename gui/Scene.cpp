#include "Scene.h"
#include "Widget.h"
#include "MouseEvent.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
    
    
void Scene::render()
{    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(auto widget : _widgets)
    {
        glPushMatrix();
        auto position = widget->position();
        glTranslated(position.x, position.y, 0.0);
        widget->render();
        glPopMatrix();
    }
}


void Scene::mousePressEvent(MouseEvent* event)
{
    event->scene = this;
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *event -= w->position();
        w->mousePressEvent(event);
    }
}

    
void Scene::mouseReleaseEvent(MouseEvent* event)
{
    event->scene = this;
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *event -= w->position();
        w->mouseReleaseEvent(event);
    }
}
    
    
void Scene::mouseMoveEvent(MouseEvent* event)
{
    event->scene = this;
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *event -= w->position();
        w->mouseMoveEvent(event);
    }
}


Widget* Scene::widgetAt(float x, float y)
{
    for(auto widget : _widgets)
    {
        auto r = widget->rect();
        if(r.overlaps(x, y))
        {
            return widget;
        }
    }
    
    return nullptr;
}
    
}//namespace r64fx
