#ifndef R64FX_MACHINE_SCENE_H
#define R64FX_MACHINE_SCENE_H

#include "Scene.h"
#include "MachineWidget.h"

namespace r64fx{
    
/** @brief Scene with machines and wires. Front and back. */
class MachineScene : public Scene{
    bool _is_showing_front_side = true;
    
public:
    inline bool isShowingFrontSide() const { return _is_showing_front_side; }
    inline void showFront(bool b = true) { _is_showing_front_side = b; }
    inline void showBack(bool b = true) { showBack(!b); }
    
    
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_SCENE_H