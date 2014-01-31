#include "Scene.h"
#include "Widget.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
    
    
void Scene::render(RenderingContextId_t context_id)
{    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(auto widget : _widgets)
    {
        glPushMatrix();
        auto position = widget->position();
        glTranslated(position.x, position.y, 0.0);
        widget->render(context_id);
        glPopMatrix();
    }
}


void Scene::mousePressEvent(MouseEvent* event)
{
    event->setScene(this);
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *event -= w->position();
        w->mousePressEvent(event);
    }
}

    
void Scene::mouseReleaseEvent(MouseEvent* event)
{
    event->setScene(this);
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *event -= w->position();
        w->mouseReleaseEvent(event);
    }
}
    
    
void Scene::mouseMoveEvent(MouseEvent* event)
{
    event->setScene(this);
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *event -= w->position();
        w->mouseMoveEvent(event);
    }
}


void Scene::mouseWheelEvent(MouseEvent* event)
{
    event->setScene(this);
}


void Scene::keyPressEvent(KeyEvent* event)
{
    event->setScene(this);
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *(event) -= w->position();
        w->keyPressEvent(event);
    }
}
    
    
void Scene::keyReleaseEvent(KeyEvent* event)
{
    event->setScene(this);
    
    Widget* w = widgetAt(event->x(), event->y());
    if(w)
    {
        *(event) -= w->position();
        w->keyReleaseEvent(event);
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


void Scene::updateWidgets()
{
    for(auto w : _widgets)
    {
        w->update();
    }
}
    
}//namespace r64fx
