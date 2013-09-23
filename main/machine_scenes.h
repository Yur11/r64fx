#ifndef R64FX_MACHINE_SCENES_H
#define R64FX_MACHINE_SCENES_H

#include "Scene.h"
#include "machine_widgets.h"

namespace r64fx{
    

class MachineScene : public Scene{
    std::vector<MachineWidget*> selected_widgets;
    /** Current mouse position. Updated with every mouse move event. */
    Point<float> mouse_position = {0.0, 0.0};
    /** Kind of like mouse position, but updated only when drag is in progress an is set to {0.0, 0.0}  upon mouse release event.*/
    Point<float> drag_position = {0.0, 0.0};
    /** Position of the mouse at the moment the drag was started. Is set to {0.0, 0.0}  upon mouse release event.*/
    Point<float> drag_start_position = {0.0, 0.0};
    bool drag_in_progress = false;
    /** Rubberband selection rect. */
    Rect<float> rubberband = {0.0, 0.0, 0.0, 0.0};

    bool can_drop = false;
    
public:
    MachineScene* counterpart_scene = nullptr;
    
    virtual void render();

    virtual void mousePressEvent(MouseEvent*);
    
    virtual void mouseReleaseEvent(MouseEvent*);
    
    virtual void mouseMoveEvent(MouseEvent*);
    
    virtual void keyPressEvent(KeyEvent*);
    
    virtual void keyReleaseEvent(KeyEvent*);
    
    void selectWidget(MachineWidget* widget);
    
    void deselectAllWidgets();
    
    void startDrag();
};
    
    
/** @brief Scene with machines and wires. Front. */
class FrontMachineScene : public MachineScene{    
};


class BackMachineScene : public MachineScene{
};


    
}//namespace r64fx

#endif//R64FX_MACHINE_SCENES_H