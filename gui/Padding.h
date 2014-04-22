#ifndef R64FX_GUI_PADDING_H
#define R64FX_GUI_PADDING_H

namespace r64fx{
    
class Padding{
    Rect<float> _padding;
    
public:
    inline float paddingLeft()   const { return _padding.left; }
    inline void setPaddingLeft(float padding) { _padding.left = padding; }
    
    inline float paddingTop()    const { return _padding.top; }
    inline void setPaddingTop(float padding) { _padding.top = padding; }
    
    inline float paddingRight()  const { return _padding.right; }
    inline void setPaddingRight(float padding) { _padding.right = padding; }
    
    inline float paddingBottom() const { return _padding.bottom; }
    inline void setPaddingBottom(float padding) { _padding.bottom = padding; }
    
    inline Rect<float> padding() const { return _padding; }
    inline void setPadding(Rect<float> padding) { _padding = padding; }
    inline void setPadding(float left, float top, float right, float bottom) { _padding = Rect<float>(left, top, right, bottom); }
    inline void setPadding(float padding) { _padding = Rect<float>(padding, padding, padding, padding); }
};
    
}//namespace r64fx

#endif//R64FX_GUI_PADDING_H