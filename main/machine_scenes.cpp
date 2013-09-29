#include "machine_scenes.h"
#include "Keyboard.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
void MachineScene::render()
{
    Scene::render();
    if(!selected_widgets.empty())
    {
        glColor3f(0.0, 0.0, 1.0);
        for(auto w : selected_widgets)
        {
            auto position = w->position();
            auto size = w->size();
            
            glBegin(GL_LINE_LOOP);
                glVertex2f(position.x, position.y);
                glVertex2f(position.x + size.w, position.y);
                glVertex2f(position.x + size.w, position.y + size.h);
                glVertex2f(position.x, position.y + size.h);
            glEnd();
        }
        
        if(drag_in_progress)
        {
            if(can_drop)
                glColor3f(0.0, 1.0, 0.0);
            else
                glColor3f(1.0, 0.0, 0.0);
            
            for(auto w : selected_widgets)
            {
                auto position = w->position() + drag_position - drag_start_position;
                auto size = w->size();
                
                glBegin(GL_LINE_LOOP);
                    glVertex2f(position.x, position.y);
                    glVertex2f(position.x + size.w, position.y);
                    glVertex2f(position.x + size.w, position.y + size.h);
                    glVertex2f(position.x, position.y + size.h);
                glEnd();
            }
        }
    }
    
    if(rubberband_in_progress && rubberband.isVisible())
    {
        auto rect = rubberband.rect();
        
        glColor3f(1.0, 1.0, 0.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(rect.left, rect.top);
            glVertex2f(rect.right, rect.top);
            glVertex2f(rect.right, rect.bottom);
            glVertex2f(rect.left, rect.bottom);
        glEnd();
    }
}


void MachineScene::mousePressEvent(MouseEvent* event)
{
    mouse_position = counterpart_scene->mouse_position = event->position();
    Scene::mousePressEvent(event);
    if(!event->has_been_handled)
    {
        deselectAllWidgets();
        counterpart_scene->deselectAllWidgets();
        rubberband_in_progress = true;
        rubberband.start(event->position());
    }
}


void MachineScene::mouseReleaseEvent(MouseEvent* event)
{
    mouse_position = counterpart_scene->mouse_position = event->position();
    if(drag_in_progress)
    {
        endDrag();
        counterpart_scene->endDrag();
    }
    else if(rubberband_in_progress)
    {
        rubberband_in_progress = false;
    }
    else
    {
        Scene::mouseReleaseEvent(event);
    }
}
    
    
void MachineScene::mouseMoveEvent(MouseEvent* event)
{
    mouse_position = counterpart_scene->mouse_position = event->position();
    
    if(drag_in_progress)
       processDrag();
    else if(rubberband_in_progress && Mouse::buttons() & Mouse::Button::Left)
        rubberband.move(event->position());
    else
        Scene::mouseMoveEvent(event);
}
    
    
void MachineScene::keyPressEvent(KeyEvent* event)
{
    Scene::keyPressEvent(event);
    if(event->has_been_handled)
        return;
    
    if(event->key() == Keyboard::Key::Tab)
    {
#ifdef DEBUG
        cout << "tab: " << this << "\n";
        assert(event->view() != nullptr);
#endif//DEBUG
        replace_me_callback(event->view(), this, counterpart_scene);
    }
}

    
void MachineScene::keyReleaseEvent(KeyEvent* event)
{
    Scene::keyReleaseEvent(event);
}


void MachineScene::selectWidget(MachineWidget* widget)
{
    if(!(Keyboard::modifiers() & Keyboard::Modifier::Ctrl))
        deselectAllWidgets();
    
    if(find(selected_widgets.begin(), selected_widgets.end(), widget) == selected_widgets.end())
        selected_widgets.push_back(widget);
}
    

void MachineScene::deselectAllWidgets()
{
    selected_widgets.clear();
}


void MachineScene::startDrag()
{
    drag_start_position = drag_position = mouse_position;
    drag_in_progress = true;
}


void MachineScene::processDrag()
{
    can_drop = true;
    for(auto w : Scene::widgets())
    {
        for(auto sw : selected_widgets)
        {       
            if(w == sw)
                break;
            
            if((sw->rect() + drag_position - drag_start_position).overlaps(w->rect()))
            {
                can_drop = false;
                goto _proceed;
            }
        }
    }
    _proceed:
    
    drag_position = counterpart_scene->drag_position = mouse_position;
    counterpart_scene->can_drop = can_drop;
}


void MachineScene::endDrag()
{
    if(can_drop)
    {
        for(auto w : selected_widgets)
        {
            w->setPosition(w->position() + drag_position - drag_start_position);
        }
    }
    
    drag_position = {0.0, 0.0};
    drag_start_position = {0.0, 0.0};
    drag_in_progress = false;
    can_drop = false;
}

    
}//namespace r64fx
