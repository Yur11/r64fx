#ifndef R64FX_GUI_SCENE_H
#define R64FX_GUI_SCENE_H

#include "Widget.h"
#include "Message.h"

namespace r64fx{
        
class View;
    
/** @brief A collection of widgets that can be rendered together and share input events. */
class Scene{
    std::vector<Widget*> _widgets;
            
public:
    bool is_scalable = true;
    
    /** @brief A request callback to replace this scene with another in the host View. */
    void (*replace_me_callback)(View* view, Scene* old_scene, Scene* new_scene);
    
    inline void appendWidget(Widget* widget) { _widgets.push_back(widget); widget->_scene = this; }
    
    inline void insertWidget(Widget* widget, int index) { _widgets.insert(_widgets.begin() + index, widget); widget->_scene = this; }
    
    inline void removeWidget(int index) { auto it = _widgets.begin() + index; (*it)->_scene = nullptr; _widgets.erase(it); }
  
    virtual void render();
    
    virtual void mousePressEvent(MouseEvent*);
    
    virtual void mouseReleaseEvent(MouseEvent*);
    
    virtual void mouseMoveEvent(MouseEvent*);
    
    virtual void mouseWheelEvent(MouseEvent*);
    
    virtual void keyPressEvent(KeyEvent*);
    
    virtual void keyReleaseEvent(KeyEvent*);
    
    Widget* widgetAt(float x, float y);
    
    void updateWidgets();
    
    template<typename T> T to() const { return (T)this; }
};

}//namespace r64fx

#endif//R64FX_GUI_SCENE_H