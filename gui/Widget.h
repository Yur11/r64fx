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
    
class MouseEvent;
class KeyEvent;
class Window;

class Widget{
    friend class Scene;
    Point<float> _position;
    Size<float> _size;
    
    Point<float> projected_position; //In window coordinates.

    Widget* _parent = nullptr;
        
    Window* _window;
    
protected:
    std::vector<Widget*> _children;
    
    std::vector<Widget*> visible_children;
    
public:
    Widget(Widget* parent = nullptr) { setParent(parent); }
    
    virtual ~Widget(){}
    
    /** @brief Set a new parent for the widget. 
     
     This will add this widget to the list of children of the parent widget if the given parent is not nullptr.
     Use nullptr to detach the widget from any parent.
     */
    void setParent(Widget* parent);
    
    /** @brief Add child widget at the end of the list. 
     
        This widget will be set as a parent of the given widget.
     */
    virtual void appendWidget(Widget* widget);
    
    /** @brief Insert child widget before the given index. Make sure the index is valid.
     
        This widget will be set as a parent of the given widget.
     */
    void insertWidget(Widget* widget, int index = 0);
    
    
    /** @brief The n'th child of the widget. 
     
        WARNING: No boundary check is done. Allways check the childrenCount().
     */
    inline Widget* child(unsigned int i) const { return _children[i]; };

    
    /** @brief Find a child in the local coordinte system of this widget. 
     
        @return Returns a widget or nullptr.
     */
    Widget* childAt(float x, float y);
    
    /** @brief Find a child in the local coordinte system of this widget. 
     
        Overloaded for convenience.
     */
    inline Widget* childAt(Point<float> p) { return childAt(p.x, p.y); }
    
    /** @brief Remove child at the given position. */
    inline void removeWidget(int i) { child(i)->setParent(nullptr); }

    inline bool hasChildren() const { return _children.empty(); }
    
    inline unsigned int childrenCount() const { return _children.size(); }
    
    /** WARNING: Can only be called if the widget has children! */
    inline Widget* lastChild() const { return _children.back(); }
    
    /** WARNING: Can only be called if the widget has children! */
    inline Widget* firstChild() const { return _children.front(); }
    
    /** @brief Remove all child widgets from this widget. */
    void clear();
    
    
    /** @brief Set a new position for the widget. 
     
        Position defines the coordintes of the bottom left corner of the widget 
        within the local coordinte system of the parent widget.
        This value is used by Widget::render_children() method.
        For widgets without a parent, position has no meaning.
     */
    inline void setPosition(Point<float> p) { _position = p; }
    
    /** @brief Set a new position for the widget. 
     
        Overloaded for convenience.
     */
    inline void setPosition(float x, float y) { setPosition(Point<float>(x, y)); };
    
    /** @brief  Coordintes of the top left corner of the widget ,
        within the local coordinte system of the parent widget.
        This value is used by Widget::render_children() method.
        For widgets without a parent, position has no meaning.
    */
    inline Point<float> position() const { return _position; };
    
    /** @brief Set the x coordinte for the left edge of the bounding rect. */
    inline void setX(float x) { _position.x = x; }
    
    /** @brief Set the y coordinte for the top edge of the bounding rect. */
    inline void setY(float y) { _position.y = y; }
    
    /** @brief The x coordinte of the left edge of the bounding rect. */
    inline float x() const { return _position.x; }
    
    /** @brief The y coordinte of the bottom edge of the bounding rect. */
    inline float y() const { return _position.y; }
    
    inline Point<float> projectedPosition() const { return projected_position; }
    
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
    
    /** @brief Widget's bounding rect. */
    inline Rect<float> boundingRect() const { return Rect<float>(position(), size()); }
    
    /** @brief Widget's rect in window coordinates. 
     
        Call project to recalculate.
     */
    inline Rect<float> projectedRect() const { return Rect<float>(projectedPosition(), size()); }
    
    /** @brief Draw the widget and it's visible children. */
    virtual void render();
    
    /** @brief Update the list of visible widgets. */
    virtual void clip(Rect<float> rect);
    
    /** @brief Recursivly calculate window coordinates for the widget tree. */
    virtual void project(Point<float> offset);
    
    /** @brief Recursivly (re)upload data to video memory for all visible widgets. */
    virtual void updateVisuals();
        
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
    
    /** @brief Convert point from this widgets coordinte system to the coordinte system of a widget up the tree. */
    Point<float> toSuperCoordinates(Point<float> point, Widget* super);
    
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
    
private:
    Widget* findLeafAt(float x, float y);
};
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H