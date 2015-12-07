#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Color.hpp"
#include "Rect.hpp"
#include "Orientation.hpp"

namespace r64fx{

class Image;
class Window;

class Painter{
//     void* m_private = nullptr;
    Point<int> m_offset  = {0, 0};

protected:
    virtual ~Painter() {};

public:
    static Painter* newInstance(Window* window);

    static void destroyInstance(Painter* painter);

    inline void setOffset(Point<int> offset) { m_offset = offset; }

    inline Point<int> offset() const { return m_offset; }

    virtual void setClipRect(Rect<int> rect) = 0;

    virtual void fillRect(Color<unsigned char> color, Rect<int> rect) = 0;

    virtual void putImage(Image* img, Point<int> pos = {0, 0}) = 0;

    virtual void repaint() = 0;

    /** @brief Adjust for the new window configuration.

        Call this if the window has been resized.
     */
    virtual void reconfigure() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP