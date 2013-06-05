#ifndef R64FX_GUI_SCENE_H
#define R64FX_GUI_SCENE_H

#include "Widget.h"

namespace r64fx{
        
/** @brief A collection of widgets that can be rendered together and share input events. */
class Scene{
    std::vector<Widget*> _widgets;
            
public:
    inline void appendWidget(Widget* widget) { _widgets.push_back(widget); }
    
    inline void insertWidget(Widget* widget, int index) { _widgets.insert(_widgets.begin() + index, widget); }
    
    inline void removeWidget(int index) { _widgets.erase(_widgets.begin() + index); }
  
    void render();
    
    void mousePressEvent(MouseEvent*);
    
    void mouseReleaseEvent(MouseEvent*);
    
    void mouseMoveEvent(MouseEvent*);
    
    Widget* widgetAt(float x, float y);  
};

}//namespace r64fx

#endif//R64FX_GUI_SCENE_H