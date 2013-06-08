#ifndef R64FX_GUI_VIEW_H
#define R64FX_GUI_VIEW_H

#include "Scene.h"

namespace r64fx{
    
/** @brief Base class for views. 
 
    We can either have a normal view that displays a scene, 
    or a splitted view, that hosts two other views. (Think Blender UI)
 */
class SplittableView{
protected:
    Rect<int> _rect;
    
public:    
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
};


/** @brief View that can be splitted into two parts. */
class SplitView : public SplittableView{
    float _split_ratio;
    SplittableView* _a;
    SplittableView* _b;

protected:
    virtual void render_separator() = 0;
    
public:
    inline void setSplitRatio(float ratio) { _split_ratio = ratio; }
    inline float splitRatio() const { return _split_ratio; }
    
    inline void setViewA(SplittableView* a) { _a = a;}
    inline void setViewB(SplittableView* b) { _b = b;}
    
    inline SplittableView* viewA() const { return _a; }
    inline SplittableView* viewB() const { return _b; }
    
    virtual void render();
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
};


class VerticalSplitView : public SplitView{
protected:
    virtual void render_separator();
    
public:
    virtual void resize(int left, int top, int right, int bottom);
};


class HorizontalSplitView : public SplitView{
protected:
    virtual void render_separator();
    
public:
    virtual void resize(int left, int top, int right, int bottom);
};

    
/** @brief A view that can render a scene with a given projection. */
class View : public SplittableView{
    
private:
    friend class Window;
    Scene* _scene = nullptr;
    float _scale_factor = 1.0;
    Point<float> _offset;
    
    void transform_mouse_event(MouseEvent* event);
    
public:
    View(Scene* scene = nullptr);
    
    virtual void resize(int left, int top, int right, int bottom);
    
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
    
    VerticalSplitView* splitVertically(float ratio);
    
    HorizontalSplitView* splitHorizontally(float ratio);
};
    
}//namespace r64fx

#endif//R64FX_GUI_VIEW_H