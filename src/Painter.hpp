#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Color.hpp"
#include "Rect.hpp"
#include "Orientation.hpp"

namespace r64fx{

class Image;
class Window;

class Painter{
    Point<int> m_offset  = {0, 0};

protected:
    virtual ~Painter() {};

public:
    static Painter* newInstance(Window* window);

    static void deleteInstance(Painter* painter);

    inline void setOffset(Point<int> offset) { m_offset = offset; }

    /** @brief Set current offset value.
        Positive x,y values denote an offset
        towards the bottom right corner of the window.
     */
    inline Point<int> offset() const { return m_offset; }

    /** @brief Set current clipping rect.

        NOTE The clipping rect postion is always set in
        the coordinate system of the window.
        The offset is not applied to it.
     */
    virtual void setClipRect(Rect<int> rect) = 0;

    /** @brief        Fill a rectangle with the given color.

        @param color  Color to use.

        @param rect   Rectangle coords. to fill.
                      Offset and clipping are applied.
                      to the rectange before it's used.
     */
    virtual void fillRect(const Rect<int> &rect, unsigned char* color) = 0;

    /** @brief        Put an image at the given position.

        @param image  Image to use. Must not be null.
                      For now RGBA only!

        @param pos    Position to put the image at.
                      Top left corner.
                      Offset and clipping are applied.
     */
    virtual void putImage(Image* img, Point<int> pos = {0, 0}) = 0;


    /* Blend multiple colors using multi-component alpha mask. */
    virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask) = 0;

    /** @brief Make the changes visible.

        Update window surface. Swap buffers. etc.
        Depending on the window type.
     */
    virtual void repaint(Rect<int>* rects = nullptr, int numrects = 0) = 0;

    /* Allways call this before any other paint commands
     * if the window has been resized. */
    virtual void adjustForWindowSize() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP