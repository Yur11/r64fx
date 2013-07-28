#ifndef R64FX_GUI_MENU_H
#define R64FX_GUI_MENU_H

#include "containers.h"

namespace r64fx{
    
class Menu : public VerticalContainer{
public:
    Menu(Widget* parent = nullptr);
};
    
}//namespace r64fx

#endif//R64FX_GUI_MENU_H