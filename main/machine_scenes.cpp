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
    
    
void MachineScene::keyPressEvent(KeyEvent* event)
{
    if(event->key() == Keyboard::Key::Tab)
    {
#ifdef DEBUG
        cout << "tab: " << this << "\n";
        assert(event->mouse_event->view != nullptr);
#endif//DEBUG
        replace_me_callback(event->mouse_event->view, this, counterpart_scene);
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

    
}//namespace r64fx
