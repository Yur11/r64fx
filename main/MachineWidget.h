#ifndef R64FX_MACHINE_WIDGET_H
#define R64FX_MACHINE_WIDGET_H

#include "Widget.h"

namespace r64fx{
    
/** @brief A machine shown in the MachineScene. */
class MachineWidget : public Widget{
    Widget* _front;
    Widget* _back;
    
public:
    MachineWidget(Widget* parent = nullptr);
    
    virtual ~MachineWidget();
    
    virtual void render();
    
    virtual void update();
    
    inline const Widget* front() const { return _front; }
    
    inline const Widget* back() const { return _back; }
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_WIDGET_H