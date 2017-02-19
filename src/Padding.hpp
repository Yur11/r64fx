#ifndef R64FX_PADDING_HPP
#define R64FX_PADDING_HPP

namespace r64fx{

template<typename T> class Padding{
    T m_left    = T();
    T m_top     = T();
    T m_right   = T();
    T m_bottom  = T();

public:
    inline T paddingLeft()   const { return m_left; }
    inline void setPaddingLeft(T padding) { m_left = padding; }

    inline T paddingTop()    const { return m_top; }
    inline void setPaddingTop(T padding) { m_top = padding; }

    inline T paddingRight()  const { return m_right; }
    inline void setPaddingRight(T padding) { m_right = padding; }

    inline T paddingBottom() const { return m_bottom; }
    inline void setPaddingBottom(T padding) { m_bottom = padding; }

    inline void setPadding(T left, T top, T right, T bottom)
    {
        m_left    = left;
        m_top     = top;
        m_right   = right;
        m_bottom  = bottom;
    }

    inline void setPadding(T padding) { setPadding(padding, padding, padding, padding); }

    inline T paddingWidth() const { return paddingLeft() + paddingRight(); }

    inline T paddingHeight() const { return paddingTop() + paddingBottom(); }
};

}//namespace r64fx

#endif//R64FX_PADDING_HPP