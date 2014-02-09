#ifndef R64FX_GUI_VIEW_H
#define R64FX_GUI_VIEW_H

#include "Scene.h"
#include "Menu.h"
#include "MouseEvent.h"


namespace r64fx{
    
class Window;
class SplitView;
class VerticalSplitView;
class HorizontalSplitView;
    
/** @brief Base class for views. 
 
    We can either have a normal view that displays a scene, 
    or a splitted view, that hosts two other views. (Think Blender UI)
 */
class SplittableView : public Disposable{
    friend class SplitView;
    
protected:
    Rect<int> _rect;
    
public:     
    virtual ~SplittableView(){}
    
    void findParentViewOrWindow(SplitView* &view, Window* &window);
    
    inline Rect<int> rect() const { return _rect; }
    
    inline int x() const { return _rect.x(); }
    
    inline int y() const { return _rect.y(); }
    
    inline int width() const { return _rect.width(); }
    
    inline int height() const { return _rect.height(); }
    
    virtual void resize(int left, int top, int right, int bottom) = 0;
    
    virtual void render() = 0;
    
    virtual void mousePressEvent(MouseEvent* event) = 0;
    
    virtual void mouseReleaseEvent(MouseEvent* event) = 0;
    
    virtual void mouseMoveEvent(MouseEvent* event) = 0;
    
    virtual void mouseWheelEvent(MouseEvent* event) = 0;
    
    virtual void keyPressEvent(KeyEvent* event) = 0;
    
    virtual void keyReleaseEvent(KeyEvent* event) = 0;

    SplittableView* findParentForView(SplittableView* view);
};

    
/** @brief A view that can render a scene with a given projection. */
class View : public SplittableView{
    friend class Window;
    Scene* _scene = nullptr;
    float _scale_factor = 1.0;
    Point<float> _scale_center = {0.0, 0.0};
    Point<float> _offset = {0.0, 0.0};
    
    Action* split_view_vert_act;
    Action* split_view_hor_act;
    Action* close_view_act;
    Menu* _context_menu;
    
    void transform_event(Event* event);
    
    View(const View&){}
    
public:
    static Icon split_view_vert_icon;
    static Icon split_view_hor_icon;
    static Icon close_view_icon;
    
    View(Scene* scene = nullptr);
    
    virtual ~View();
    
    virtual void resize(int left, int top, int right, int bottom);
    
    void setScene(Scene* scene);
    
    inline Scene* scene() const { return _scene; }
    
    inline void setOffset(Point<float> offset) { _offset = offset; }
    
    inline void setOffset(float x, float y) { setOffset(Point<float>(x, y)); }
    
    inline void translate(Point<float> p) { _offset += p; }
    
    inline void translate(float x, float y) { translate(Point<float>(x, y)); }
    
    inline Point<float> offset() const { return _offset; }
    
    /** @brief Set the scale factor of the view. */
    inline void setScaleFactor(float scale_factor) { _scale_factor = scale_factor; }

    /** @brief Multiply the scale factor by a given value. */
    inline void scale(float coeff) { _scale_factor *= coeff; }
    
    inline float scaleFactor() const { return _scale_factor; }
      
    virtual void render();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void mouseWheelEvent(MouseEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    virtual void keyReleaseEvent(KeyEvent* event);
    
    /** @brief Transform event from the window coordinates to the coordinate system of the View. 
     
        This includes scale and translation.
     */
    void transformEvent(Event* event);
    
    VerticalSplitView* splitVertically(float ratio);
    
    HorizontalSplitView* splitHorizontally(float ratio);
    
    void zoomOnce(float scale_coeff, Point<float> mouse_position = {0.0, 0.0});
    
    inline void zoomInOnce(Point<float> mouse_position = {0.0, 0.0}) { zoomOnce(1.1, mouse_position); }
    
    inline  void zoomOutOnce(Point<float> mouse_position = {0.0, 0.0}) { zoomOnce(1.0 / 1.1, mouse_position); }
};



/** @brief View that can be splitted into two parts. */
class SplitView : public SplittableView{
    float _split_ratio = 0.5;
    bool _separator_is_hovered = false;
    bool _separator_is_grabbed = false;
    SplittableView* _a = nullptr;
    SplittableView* _b = nullptr;

protected:
    virtual void set_separator_coords() = 0;
    
    virtual Rect<float> separatorRect() = 0;
    
    virtual void moveSeparator(Point<float> p) = 0;
    
public:
    virtual ~SplitView() {}
    
    inline bool separatorIsHovered() const { return _separator_is_hovered; }
    inline bool separatorIsGrabbed() const { return _separator_is_grabbed; }
    
    inline void setSplitRatio(float ratio) { _split_ratio = ratio; }
    inline float splitRatio() const { return _split_ratio; }
    
    inline void setViewA(SplittableView* a) { _a = a; }
    inline void setViewB(SplittableView* b) { _b = b; }
    
    /** @brief Replace one of the sub-views. */
    void replaceSubView(SplittableView* old_view, SplittableView* new_view);
    
    inline SplittableView* viewA() const { return _a; }
    inline SplittableView* viewB() const { return _b; }
    
    virtual void render();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void mouseWheelEvent(MouseEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    virtual void keyReleaseEvent(KeyEvent* event);
};


class VerticalSplitView : public SplitView{
protected:
    virtual void set_separator_coords();
    
    virtual Rect<float> separatorRect();
    
    virtual void moveSeparator(Point<float> p);
    
public:
    virtual ~VerticalSplitView() {}
    
    virtual void resize(int left, int top, int right, int bottom);
};


class HorizontalSplitView : public SplitView{
protected:
    virtual void set_separator_coords();
    
    virtual Rect<float> separatorRect();
    
    virtual void moveSeparator(Point<float> p);
    
public:
    virtual ~HorizontalSplitView() {}
    
    virtual void resize(int left, int top, int right, int bottom);
};
    
}//namespace r64fx

#endif//R64FX_GUI_VIEW_H