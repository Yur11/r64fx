#ifndef R64FX_MAIN_INFORMATION_SCENE_H
#define R64FX_MAIN_INFORMATION_SCENE_H

#include "gui/Scene.h"
#include "gui/containers.h"
#include "gui/Painter.h"

namespace r64fx{
    
/** @brief Scene that host the main panel. */
class InformationScene : public Scene{
    
public:
    class Panel : public HorizontalContainer{
        Painter p;
        
    public:
        Panel();
        
        virtual void render();
        
        void update();
    } *panel;
    
    InformationScene();
};
    
}//namespace r64fx

#endif//R64FX_MAIN_INFORMATION_SCENE_H