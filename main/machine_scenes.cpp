#include "machine_scenes.h"
#include "Keyboard.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
void MachineScene::render()
{
    Scene::render();
    if(dragged_widget)
    {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(
                mouse_position.x - dragged_widget->drag_start_mouse_position.x,
                mouse_position.y - dragged_widget->drag_start_mouse_position.y
            );
            
            glVertex2f(
                mouse_position.x - dragged_widget->drag_start_mouse_position.x + dragged_widget->width(),
                mouse_position.y - dragged_widget->drag_start_mouse_position.y
            );
            
            glVertex2f(
                mouse_position.x - dragged_widget->drag_start_mouse_position.x + dragged_widget->width(),
                mouse_position.y - dragged_widget->drag_start_mouse_position.y + dragged_widget->height()
            );
            
            glVertex2f(
                mouse_position.x - dragged_widget->drag_start_mouse_position.x,
                mouse_position.y - dragged_widget->drag_start_mouse_position.y + dragged_widget->height()
            );
        glEnd();
    }
}


void MachineScene::mouseReleaseEvent(MouseEvent* event)
{
    if(dragged_widget)
    {
        dragged_widget->setPosition(event->position() - dragged_widget->drag_start_mouse_position);
        counterpart_scene->dragged_widget->setPosition(event->position() - dragged_widget->drag_start_mouse_position);
        
        dragged_widget = nullptr;
        counterpart_scene->dragged_widget = nullptr;
    }
    else
    {
        Scene::mouseReleaseEvent(event);
    }
}
    
    
void MachineScene::mouseMoveEvent(MouseEvent* event)
{
    mouse_position = event->position();
    counterpart_scene->mouse_position = event->position();
    
    if(dragged_widget)
    {
        cout << "drag: " << event->position().x << ", " << event->position().y << "\n";
        /* Check if widget can be dropped here. */
    }
    else
    {
        Scene::mouseMoveEvent(event);
    }
}
    
    
void MachineScene::keyPressEvent(KeyEvent* event)
{
    if(event->key() == Keyboard::Key::Tab)
    {
#ifdef DEBUG
        cout << "tab: " << this << "\n";
        assert(event->view() != nullptr);
#endif//DEBUG
        replace_me_callback(event->view(), this, counterpart_scene);
    }
    else
    {
        Scene::keyPressEvent(event);
    }
}

    
void MachineScene::keyReleaseEvent(KeyEvent* event)
{
    Scene::keyReleaseEvent(event);
}


void MachineScene::startDrag(MachineWidget* own_dragged_widget, MachineWidget* counterpart_scene_widget)
{
    dragged_widget = own_dragged_widget;
    counterpart_scene->dragged_widget = counterpart_scene_widget;
    counterpart_scene->dragged_widget->drag_start_mouse_position = own_dragged_widget->drag_start_mouse_position;
}

    
}//namespace r64fx
