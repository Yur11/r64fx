#ifndef R64FX_MAIN_MACHINE_SCENE_H
#define R64FX_MAIN_MACHINE_SCENE_H

#include "gui/Scene.h"
#include "MachineWidget.h"
#include "Wire.h"

namespace r64fx{
    

/** @brief Base class for the FrontMachineScene and the BackMachineScene. */
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
    Rubberband rubberband;
    bool rubberband_in_progress = false;
    
    bool can_drop = false;

protected:    
    void render_wires(RenderingContextId_t context_id);
    
public:
    /* Making this thing public for debugging purposes. */
    std::vector<Wire*>* wires;
    
    void updateWires();
    
    MachineScene* counterpart_scene = nullptr;
    
    virtual void render(RenderingContextId_t context_id);

    virtual void mousePressEvent(MouseEvent*);
    
    virtual void mouseReleaseEvent(MouseEvent*);
    
    virtual void mouseMoveEvent(MouseEvent*);
    
    virtual void keyPressEvent(KeyEvent*);
    
    virtual void keyReleaseEvent(KeyEvent*);
    
    void selectWidget(MachineWidget* widget);
    
    void deselectAllWidgets();
    
    void startDrag();
    
    void processDrag();
    
    void endDrag();
};
    
    
/** @brief Scene with machines and wires. Front. */
class FrontMachineScene : public MachineScene{
public:
    virtual void render(RenderingContextId_t context_id);
};


/** @brief Scene with machines and wires. Back. */
class BackMachineScene : public MachineScene{
public:
    virtual void render(RenderingContextId_t context_id);
};


    
}//namespace r64fx

#endif//R64FX_MAIN_MACHINE_SCENE_H