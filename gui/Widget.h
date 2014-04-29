#ifndef R64FX_GUI_WIDGET_H
#define R64FX_GUI_WIDGET_H

#include <vector>
#include "utf8string/Utf8String.h"
#include "gl.h"
#include "Rect.h"
#include "RenderingContext.h"
#include "Projection2D.h"
#include "IteratorPair.h"


namespace r64fx{

class Widget;    
class MouseEvent;
class KeyEvent;
class Window;

typedef IteratorPair<std::vector<Widget*>::iterator>  WidgetIteratorPair;

class Widget{
    friend class Scene;
    Point<float> relative_position; //Parent coordinates.
    Size<float>  _size;

    Widget* _parent = nullptr;
        
    Window* _window;
    
protected:
    /* These are made protected to allow alternative more efficient 
     * implementations of algorithms that use these fields. */
    
    std::vector<Widget*> children;

    Point<float> absolute_position; //Root(Window) coordinates.
                                    //Never set directly.
                                    //Calculated by recursively accumulating relative positions.
    
    bool is_visible = true;
    
    WidgetIteratorPair visible_children;
        
    virtual void projectToRootAndClipVisible(Rect<float> rect);
    
public:
    Widget(Widget* parent = nullptr) { setParent(parent); }
    
    virtual ~Widget(){}
    
    /** @brief Set a new parent for the widget. 
     
     This will add this widget to the list of children of the parent widget if the given parent is not nullptr.
     Use nullptr to detach the widget from any parent.
     */
    void setParent(Widget* parent);
        
    inline WidgetIteratorPair allChildren() { return { children.begin(), children.end() }; }
    
    inline WidgetIteratorPair visibleChildren() { return visible_children; }
    
    inline bool isVisible() const { return is_visible; }
    
    /** @brief  */
    
    /** @brief Add child widget at the end of the list. 
     
        This widget will be set as a parent of the given widget.
     */
    virtual void appendWidget(Widget* widget);
    
    /** @brief Insert child widget before the given index. Make sure the index is valid.
     
        This widget will be set as a parent of the given widget.
     */
    void insertWidget(Widget* widget, int index = 0);
    
    inline int childrenCount() const { return children.size(); }
    
    inline bool hasChildren() const { return !children.empty(); }
    
    inline void removeChild(int i) { children[i]->setParent(nullptr); }
    
    
    /** @brief Remove all child widgets from this widget. 
     
        This also clears the visible_children list.
     */
    void clear();
    
    
    /** @brief Set a new position for the widget. 
     
        Position defines the coordintes of the bottom left corner of the widget 
        within the local coordinte system of the parent widget.
        This value is used by Widget::render_children() method.
        For widgets without a parent, position has no meaning.
     */
    inline void setRelativePosition(Point<float> p) { relative_position = p; }
    
    /** @brief Set a new position for the widget. 
     
        Overloaded for convenience.
     */
    inline void setRelativePosition(float x, float y) { setRelativePosition(Point<float>(x, y)); };

    inline void setRelativeX(float x) { relative_position.x = x; }
    inline void setRelativeY(float y) { relative_position.y = y; }
    
    /** @brief  Coordintes of the top left corner of the widget ,
        within the local coordinte system of the parent widget.
        This value is used by Widget::render_children() method.
        For widgets without a parent, position has no meaning.
    */
    inline Point<float> relativePosition() const { return relative_position; };
    
    inline Point<float> absolutePosition() const { return absolute_position; }
    
    /** @brief Resize the bounding rect of the widget. */
    inline void resize(Size<float> s) { _size = s; }
    
    /** @brief Resize the bounding rect of the widget. 
     
        Overloaded for convenience.
     */
    inline void resize(float w, float h) { resize(Size<float>(w, h)); }
    
    /** @brief Size of the widgets bounding rect. */
    inline Size<float> size() const { return _size; }

    /** @brief Set the width of the widgets bounding rect. */
    inline void setWidth(float w) { _size.w = w; }
    
    /** @brief Set the height of the widgets bounding rect. */
    inline void setHeight(float h) { _size.h = h; }
    
    /** @brief Width of the bounding rect. */
    inline float width() const { return _size.w; }
    
    /** @brief Height of the bounding rect. */
    inline float height() const { return _size.h; }
    
    inline Rect<float> relativeRect() const { return { relative_position, _size }; }
    
    inline Rect<float> absoluteRect() const { return { absolute_position, _size }; }
    
    virtual void update();
    
        
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
    virtual void textInputEvent(Utf8String text);
    
    
    /** @brief  Convert point from this widgets coordinte system to the coordinte system of the parent. 
     
        This widget must have a parent!
     */
    Point<float> toParentCoords(Point<float> point);
    
//     /** @brief Convert point from this widgets coordinte system to the coordinte system of a widget up the tree. */
//     Point<float> toSuperCoordinates(Point<float> point, Widget* super);
    
    /** @brief Convert point from this widgets coordinte system to the coordinte system of the root widget.
        
        Root widget is a widget on the top to the widget tree, that has no parent.
     
        This widget must have a parent!
     */
    Point<float> toRootCoords(Point<float> point);
    
    /** @brief Fetch the root widget of the tree that this widget belongs to. */
    Widget* root(); 

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
    
    bool isMouseGrabber();
    
    bool isKeyboardGrabber();

};


/** @brief Find a widget that overlaps the given point, using it's realative position. */
Widget* find_widget_relative(Point<float> p, WidgetIteratorPair range);


/** @brief Find a widget that overlaps the given point, using it's absolute position. */
Widget* find_widget_absolute(Point<float> p, WidgetIteratorPair range);
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H