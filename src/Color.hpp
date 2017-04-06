#ifndef R64FX_COLOR_HPP
#define R64FX_COLOR_HPP

namespace r64fx{

union Color{
    unsigned char m_color[4];
    unsigned int  m_bits = 0;

public:
    explicit Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : m_color{r, g, b, a}
    {}

    explicit Color(unsigned char r, unsigned char g, unsigned char b)
    : m_color{r, g, b, 0}
    {}

    explicit Color(unsigned char r, unsigned char g)
    : m_color{r, g, 0, 0}
    {}

    explicit Color(unsigned char r)
    : m_color{r, 0, 0, 0}
    {}

    Color() : m_color{0, 0, 0, 0}
    {}

    Color(const Color& other)
    {
        operator=(other);
    }

    inline Color &operator=(const Color& other)
    {
        m_bits = other.bits();
        return *this;
    }

    inline operator unsigned char*() { return m_color; }

    inline unsigned char operator[](int c) { return m_color[c]; }

    inline const unsigned int &bits() const { return m_bits; }
};

inline bool operator==(const Color &a, const Color &b)
{
    return a.bits() == b.bits();
}

inline bool operator!=(const Color &a, const Color &b)
{
    return !operator==(a, b);
}


class Colors{
    Color m_colors[4];

public:
    Colors(Color r, Color g, Color b, Color a)
    : m_colors{r, g, b, a}
    {}

    Colors(Color r, Color g, Color b)
    : m_colors{r, g, b, Color()}
    {}

    Colors(Color r, Color g)
    : m_colors{r, g, Color(), Color()}
    {}

    Colors(Color r)
    : m_colors{r, Color(), Color(), Color()}
    {}

    Colors()
    : m_colors{Color(), Color(), Color(), Color()}
    {}

    Colors(const Colors &other)
    {
        operator=(other);
    }

    inline Colors &operator=(const Colors &other)
    {
        for(int i=0; i<4; i++)
            m_colors[i] = other[i];
        return *this;
    }

    inline Color operator[](int i) const
    {
        return m_colors[i];
    }
};

inline bool operator!=(const Colors &a, const Colors &b)
{
    for(int i=0; i<4; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

inline bool operator==(const Colors &a, const Colors &b)
{
    return !operator!=(a, b);
}

}//namespace r64fx

#endif//R64FX_COLOR_HPP
