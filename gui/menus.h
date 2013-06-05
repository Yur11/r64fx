#ifndef R64FX_GUI_MENUS_H
#define R64FX_GUI_MENUS_H

#include "containers.h"

namespace r64fx{


class MenuSeparator : public Widget, public ColorMixin{    
};

class HorizontalMenuSeparator : public MenuSeparator{
public:
    HorizontalMenuSeparator(Color c = Color()) { setColor(c); }
    
    virtual void render();
};

class VerticalMenuSeparator : public MenuSeparator{
    
};

    
/** @brief A vertical drop-down menu. */
class VerticalMenu : public VerticalContainer{
    
public:
    VerticalMenu(Widget* parent = nullptr) : VerticalContainer(parent) {}
};


/** @brief A horizontal menu. */
class HorizontalMenu : public HorizontalContainer{
    
public:
    HorizontalMenu(Widget* parent = nullptr) : HorizontalContainer(parent) {}
};


/** @brief Context menu shown in overlays */
class ContextMenu : public VerticalMenu, public BackgroundColorMixin{
public:
    virtual void render();
};
    
}//namespace r64fx

#endif//R64FX_GUI_MENUS_H