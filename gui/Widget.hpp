#ifndef R64FX_GUI_WIDGET_H
#define R64FX_GUI_WIDGET_H

#include <vector>
#include "utf8string/Utf8String.hpp"
#include "gl.hpp"
#include "Rect.hpp"
#include "RenderingContext.hpp"
#include "Projection2D.hpp"
#include "shared_sources/LinkedItem.hpp"
#include "IteratorPair.hpp"


namespace r64fx{

class Widget;    
class MouseEvent;
class KeyEvent;
class TextInputEvent;
class Window;

typedef IteratorPair<LinkedItemPointer<Widget>> WidgetIteratorPair;

class Widget : public LinkedItem<Widget>{
    friend class Window;
    
    static Widget* mouse_grabber;
    static Widget* keyboard_grabber;

    Widget* parent_widget = nullptr;
            
    Widget(const Widget &other) {}
    
protected:
    /* These are made protected to allow alternative, more efficient 
     * implementations of algorithms that use these fields. */
    
    LinkedItemChain<Widget> children;
    
    bool is_visible = true;
    
    Rect<float> projected_rect;
    
    Rect<float> visible_rect;

    WidgetIteratorPair visible_children = { nullptr, nullptr };
    
    virtual void projectToRootAndClipVisible(Point<float> parent_position, Rect<float> parent_visible_rect);
    
public:
    Widget(Widget* parent = nullptr) { setParent(parent); }
    
    virtual ~Widget(){}
    
    /** @brief Widget's bounding rectangle in parent coordinates. 
     
        Use this to change the widget's size and position relative to it's parent.
     */
    Rect<float> rect;
    
    /** @brief Same as rect.position() */
    inline Point<float> relativePosition() const { return rect.position(); };
    
    inline void setPosition(float x, float y) { rect.setPosition(x, y); }
    
    inline void setPosition(Point<float> p) { rect.setPosition(p); }
    
    inline float width() const { return rect.width(); }
    
    inline float height() const { return rect.height(); }
    
    inline void setWidth(float width) { rect.setWidth(width); }
    
    inline void setHeight(float height) { rect.setHeight(height); }

    inline void setSize(float w, float h) { rect.setSize(w, h); }
    
    inline void setSize(Size<float> s) { rect.setSize(s); }
    
    /** @brief Widget's bounding rect projected to the coordinate system of the root widget ( i.e. the window ). */    
    inline Rect<float> projectedRect() const { return projected_rect; }
    
    /** @brief Widget's rect that is actually visible to the user. */
    inline Rect<float> visibleRect() const { return visible_rect; }
    
    /** @brief Set a new parent for the widget. 
     
     This will add this widget to the list of children of the parent widget if the given parent is not nullptr.
     Use nullptr to detach the widget from any parent.
     */
    void setParent(Widget* new_parent_widget);
        
    inline WidgetIteratorPair allChildren() { return { children.begin(), children.end() }; }
    
    inline WidgetIteratorPair visibleChildren() { return visible_children; }
    
    inline bool isVisible() const { return is_visible; }
    
    
    /** @brief Add child widget at the end of the list. 
     
        This widget will be set as a parent of the given widget.
     */
    virtual void appendWidget(Widget* widget);
    
    void insertWidget(Widget* existing_widget, Widget* widget);
        
    inline bool hasParent() const { return parent_widget != nullptr; }
    
    inline bool hasChildren() const { return !children.isEmpty(); }
        
    void removeFromParent();
    
    /** @brief Remove all child widgets from this widget. 
     
        This also clears the visible_children list.
     */
    void clear();
    
    Widget* visibleChildAt(Point<float> p);
        
protected:
    /**  */
    virtual void appearanceChangeEvent();
    
    /** @brief Draw the widget and it's visible children. */
    virtual void render();
        
    /** @brief Send a mouse press event to the widget. */
    virtual void mousePressEvent(MouseEvent* event);

    /** @brief Send a mouse release event to the widget. */
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    /** @brief Send a mouse move event to the widget. */
    virtual void mouseMoveEvent(MouseEvent* event);
    
    /** @brief Send a mouse wheel event to the widget. */
    virtual void mouseWheelEvent(MouseEvent* event);
    
    /** @brief Send a key press event to the widget. */
    virtual void keyPressEvent(KeyEvent* event);
    
    /** @brief Send a key release event to the widget. */
    virtual void keyReleaseEvent(KeyEvent* event);
    
    /** @brief Send a text input event to the widget. */
    virtual void textInputEvent(TextInputEvent* event);
    
public:
    /** @brief Set the mouse grabber to be a widget or nullptr. */
    static void setMouseGrabber(Widget* widget);
    
    /** @brief Set the keyboard grabber to be a widget or nullptr. */
    static void setKeyboardGrabber(Widget* widget);
    
    static Widget* mouseInputGrabber();
    
    static Widget* keyboardInputGrabber();
    
    /** @brief Make this widget a mouse grabber. */
    inline void grabMouseInput() { setMouseGrabber(this); }
    
    inline void ungrabMouseInput() { setMouseGrabber(nullptr); }
    
    /** @brief Make this widget a keyboard grabber. */
    inline void grabKeyboardInput() { setKeyboardGrabber(this); }
    
    /** @brief Ungrab the keyboard.  */
    inline void ungrabKeyboardInput() { setKeyboardGrabber(nullptr); }
    
    bool isMouseInputGrabber();
    
    bool isKeyboardInputGrabber();

};
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H