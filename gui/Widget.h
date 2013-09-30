#ifndef R64FX_GUI_WIDGET_H
#define R64FX_GUI_WIDGET_H

#include <vector>
#include "Utf8String.h"
#include <GL/glu.h>
#include "geometry.h"
#include "events.h"
#include "Color.h"
#include "Message.h"
#include "gc.h"


namespace r64fx{

class MouseEvent;
class KeyEvent;
class Scene;
class Window;


/** @brief Base class for UI widgets. 
 
    Widgets are used to construct the user interface of a program.
    They form a parent/child relations with each other, as well as
    with the Scene instances.
    
    Each widget has a position value that is set in the coordinte system
    of it's parent, either another widget or a Scene.
    The (0, 0) point is in the lower left corner.
    
    Each widget has a bunch of event handlers for rendering and also mouse and keyboard input.
    Reimplement these as needed.
 */
class Widget : public Disposable{
    friend class Scene;
    Point<float> _position;
    Size<float> _size;

    Widget* _parent = nullptr;
    
    /** Pointer to the scene that this widget belongs to. 
        Some widgets may have this value be null.
        However the root widget should have this pointer set to a valid address.
        This way we can "climb" up the widget tree to the root
        and obtain the scene pointer from there, without having to update this value for every child widget.
    */
    Scene* _scene = nullptr;
    
    Window* _window;
    
protected:
    void render_children();
    
    void render_bounding_rect();

    std::vector<Widget*> _children;
    
public:
    Widget(Widget* parent = nullptr) { setParent(parent); }
    
    virtual ~Widget(){}
    
    template<typename T> T to() { return (T) this; }
    
    /** @brief Set a new parent for the widget. 
     
     This will add this widget to the list of children of the parent widget if the given parent is not nullptr.
     Use nullptr to detach the widget from any parent.
     */
    void setParent(Widget* parent);
    
    /** @brief Add child widget at the end of the list. 
     
        This widget will be set as a parent of the given widget.
     */
    void appendWidget(Widget* widget);
    
    /** @brief Insert child widget before the given index. Make sure the index is valid.
     
        This widget will be set as a parent of the given widget.
     */
    void insertWidget(Widget* widget, int index = 0);
    
    /** @brief Remove child at the given position. */
    inline void removeWidget(int i) { child(i)->setParent(nullptr); }
    
    /** @brief Set a new position for the widget. 
     
        Position defines the coordintes of the top left corner of the widget 
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
    
    /** @brief The y coordinte of the top edge of the bounding rect. */
    inline float y() const { return _position.y; }
    
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
    
    /** @brief Width of the widgets bounding rect. */
    inline float width() const { return _size.w; }
    
    /** @brief Height of the widgets bounding rect. */
    inline float height() const { return _size.h; }
    
    /** @brief Widgets bounding rect. */
    inline Rect<float> rect() const { return Rect<float>(position(), size()); }
    
    /** @brief Draw the widget. 
     
        This method should be implemented by subclasses to do their rendering.
    */
    virtual void render();

    /** @brief Tell the widget to use newly added information. */
    virtual void update();
    
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
    
    /** @brief Number of children of the widget. */
    inline unsigned int childrenCount() const { return _children.size(); }
    
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

    /** @brief Convert point from this widgets coordinte system to the coordinte system of the scene it belongs to. */
    Point<float> toSceneCoords(Point<float> point);
    
    /** @brief Fetch the root widget of the tree that this widget belongs to. */
    Widget* root(); 
    
    inline Scene* scene() { return root()->_scene; }

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
    
public:
    
    static Widget* stub();
};
    
}//r64fx

#endif//R64FX_GUI_WIDGET_H