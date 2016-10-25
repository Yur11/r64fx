#ifndef R64FX_COLOR_HPP
#define R64FX_COLOR_HPP

namespace r64fx{

class Color{
    unsigned char m_color[4];

public:
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : m_color{r, g, b, a}
    {}

    Color(unsigned char r, unsigned char g, unsigned char b)
    : m_color{r, g, b, 0}
    {}

    Color(unsigned char r, unsigned char g)
    : m_color{r, g, 0, 0}
    {}

    Color(unsigned char r)
    : m_color{r, 0, 0, 0}
    {}

    inline operator unsigned char*() { return m_color; }
    
    inline unsigned char &operator[](int c) { return m_color[c]; }
};


class Colors{
    unsigned char* m_colors[4];

public:
    Colors(unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
    : m_colors{r, g, b, a}
    {}

    Colors(unsigned char* r, unsigned char* g, unsigned char* b)
    : m_colors{r, g, b, nullptr}
    {}

    Colors(unsigned char* r, unsigned char* g)
    : m_colors{r, g, nullptr, nullptr}
    {}

    Colors(unsigned char* r)
    : m_colors{r, nullptr, nullptr, nullptr}
    {}

    inline operator unsigned char**() { return m_colors; }
};

}//namespace r64fx

#endif//R64FX_COLOR_HPP
