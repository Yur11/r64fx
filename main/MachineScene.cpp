#include "MachineScene.h"
#include "gui/Keyboard.h"
#include "gui/MouseEvent.h"
#include "gui/KeyEvent.h"
#include "gui/Painter.h"
#include "gui/RectPainter.h"
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
        RectPainter::prepare();
        RectPainter::setTexture(RectPainter::plainTexture());
        RectPainter::setTexCoords(0.0, 0.0, 1.0, 1.0);
        RectPainter::setColor(0.0, 0.0, 1.0, 1.0);
         
        for(auto w : selected_widgets)
        {
            auto position = w->position();
            auto size = w->size();
            
            RectPainter::setCoords(position.x, position.y, size.w, size.h);
            RectPainter::renderOutline();
        }
        
        if(drag_in_progress)
        {
            if(can_drop)
                RectPainter::setColor(0.0, 1.0, 0.0, 1.0);
            else
                RectPainter::setColor(1.0, 0.0, 0.0, 1.0);
            
            for(auto w : selected_widgets)
            {
                auto position = w->position() + drag_position - drag_start_position;
                auto size = w->size();                
                
                RectPainter::setCoords(position.x, position.y, size.w, size.h);
                RectPainter::renderOutline();
            }
        }
    }
}


void MachineScene::render_wires()
{
    for(auto w : *wires)
        w->render();
}


void MachineScene::updateWires()
{
    for(auto w : *wires)
        w->update();
}


void MachineScene::mousePressEvent(MouseEvent* event)
{
    mouse_position = counterpart_scene->mouse_position = event->position();
    Scene::mousePressEvent(event);
    if(!event->has_been_handled)
    {
        deselectAllWidgets();
        counterpart_scene->deselectAllWidgets();
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
        assert(event->view() != nullptr);
#endif//DEBUG
        replace_me_callback(event->view(), this, counterpart_scene);
    }
}

    
void MachineScene::keyReleaseEvent(KeyEvent* event)
{
    Scene::keyPressEvent(event);
    if(event->has_been_handled)
        return;
    
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
    
    updateWires();
}


void FrontMachineScene::render()
{
    render_wires();
    MachineScene::render();
}



void BackMachineScene::handleSocketClick(Socket* socket)
{
#ifdef DEBUG
    assert(socket != nullptr);
#endif//DEBUG
    
    if(socket->isPlugged())
    {
        if(_active_sockets.empty())
        {
            auto wire = socket->wire();
            wire->unplug();
            for(auto it=wires->begin(); it!=wires->end(); it++)
            {
                if(*it == wire)
                {
                    wires->erase(it);
                    break;
                }
            }
            wire->deleteLater();
        }
    }
    else
    {
        if(_active_sockets.empty())
        {
            _active_sockets.push_back(socket);
        }
        else
        {
            auto a = socket;
            auto b = _active_sockets[0];
            
            if(a == b)
                return;
            
            auto wire = new Wire(a, b);
            wire->update();
            wire->color = { 0.7, 0.7, 0.1, 1.0 };
            wires->push_back(wire);
            
            _active_sockets.clear();
        }
    }
}


void BackMachineScene::render()
{
    MachineScene::render();
    render_wires();
}
    
}//namespace r64fx
