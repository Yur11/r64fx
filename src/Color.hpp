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

    inline operator unsigned char*() { return m_color; }

    inline unsigned char &operator[](int c) { return m_color[c]; }

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
    unsigned char* m_colors[4];

public:
    explicit Colors(unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
    : m_colors{r, g, b, a}
    {}

    explicit Colors(unsigned char* r, unsigned char* g, unsigned char* b)
    : m_colors{r, g, b, nullptr}
    {}

    explicit Colors(unsigned char* r, unsigned char* g)
    : m_colors{r, g, nullptr, nullptr}
    {}

    explicit Colors(unsigned char* r)
    : m_colors{r, nullptr, nullptr, nullptr}
    {}

    inline operator unsigned char**() { return m_colors; }
};

}//namespace r64fx

#endif//R64FX_COLOR_HPP
