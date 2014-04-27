#ifndef R64FX_GUI_PADDING_H
#define R64FX_GUI_PADDING_H

namespace r64fx{
    
template<typename T>
class Padding{
    Rect<T> _padding;
    
public:
    inline T paddingLeft()   const { return _padding.left; }
    inline void setPaddingLeft(T padding) { _padding.left = padding; }
    
    inline T paddingTop()    const { return _padding.top; }
    inline void setPaddingTop(T padding) { _padding.top = padding; }
    
    inline T paddingRight()  const { return _padding.right; }
    inline void setPaddingRight(T padding) { _padding.right = padding; }
    
    inline T paddingBottom() const { return _padding.bottom; }
    inline void setPaddingBottom(T padding) { _padding.bottom = padding; }
    
    inline Rect<T> padding() const { return _padding; }
    inline void setPadding(Rect<T> padding) { _padding = padding; }
    inline void setPadding(T left, T top, T right, T bottom) { _padding = Rect<T>(left, top, right, bottom); }
    inline void setPadding(T padding) { _padding = Rect<T>(padding, padding, padding, padding); }
    
    inline T paddingWidth() const { return paddingLeft() + paddingRight(); }
    inline T paddingHeight() const { return paddingTop() + paddingBottom(); }
};
    
}//namespace r64fx

#endif//R64FX_GUI_PADDING_H