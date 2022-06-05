#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Color.hpp"
#include "Image.hpp"
#include "ImageArgs.hpp"
#include "Offset.hpp"
#include "FlipFlags.hpp"

namespace r64fx{

class Painter;
class Window;


class PainterTexture{
protected:
    virtual ~PainterTexture() {};

public:
    virtual Painter* parentPainter() = 0;

    virtual bool isGood() = 0;

    virtual int componentCount() = 0;

    virtual void free() = 0;
};


class PainterTexture1D : public PainterTexture{
protected:
    virtual ~PainterTexture1D() {};

public:
    enum class Type{
        UnsignedChar,
        UnsignedShort,
        UnsignedInt,
        Float
    };

    virtual Type dataType() = 0;

    virtual int length() = 0;

    virtual void load(unsigned char*   data, int length, int component_count) = 0;

    virtual void load(unsigned short*  data, int length, int component_count) = 0;

    virtual void load(unsigned int*    data, int length, int component_count) = 0;

    virtual void load(float* data, int length, int component_count) = 0;
};


class PainterTexture2D : public PainterTexture{
protected:
    virtual ~PainterTexture2D() {};

public:
    virtual Size<int> size() = 0;

    virtual void allocStorage(Size<int> size, int component_count) = 0;

    virtual void loadSubImage(const Point<int> &dstpos, const ImgRect &src) = 0;

    virtual void loadImage(Image* teximg) = 0;

    virtual void readImage(Image* teximg) = 0;
};



class PainterTextureManager{
public:
    virtual PainterTexture1D* newTexture() = 0;

    virtual PainterTexture1D* newTexture(unsigned char*  data, int length, int component_count) = 0;

    virtual PainterTexture1D* newTexture(unsigned short* data, int length, int component_count) = 0;

    virtual PainterTexture1D* newTexture(unsigned int*   data, int length, int component_count) = 0;

    virtual PainterTexture1D* newTexture(float* data, int length, int component_count) = 0;

    virtual PainterTexture2D* newTexture(Image* image) = 0;

    virtual void deleteTexture(PainterTexture1D* texture) = 0;

    virtual void deleteTexture(PainterTexture2D* texture) = 0;
};


class Painter : public PainterTextureManager, public OffsetMixin<int>{

protected:
    virtual ~Painter() {};

public:
    static Painter* newInstance(Window* window);

    static void deleteInstance(Painter* painter);

    /** Set current clipping rect.

        NOTE The clipping rect postion is always set in
        the coordinate system of the window.
        The offset is not applied to it.
     */
    virtual void setClipRect(Rect<int> rect) = 0;

    virtual void setClipRectAtCurrentOffset(Size<int> size) = 0;

    virtual Rect<int> clipRect() = 0;

    virtual void clear(Color color) = 0;

    /** Fill a rectangle with the given color.

        rect   Rectangle coords. to fill.
               Offset and clipping are applied.
               to the rectange before it's used.

        color  Color to use.
     */
    virtual void fillRect(const Rect<int> &rect, Color color) = 0;

    virtual void strokeRect(const Rect<int> &rect, Color stroke, Color fill, int stroke_width = 1) = 0;

    /** Put an image at the given position.

        image  Image to use. Must not be null.
                      For now RGBA only!

        pos    Position to put the image at.
                      Top left corner.
                      Offset and clipping are applied.
     */
    virtual void putImage(Image* image, Point<int> dst_pos, Rect<int> src_rect, FlipFlags flags = FlipFlags()) = 0;

    inline void putImage(Image* image, Point<int> dst_pos, FlipFlags flags = FlipFlags())
    {
        putImage(image, dst_pos, {0, 0, image->width(), image->height()}, flags);
    }

    /**  */
    virtual void putImage(PainterTexture2D* texture, Point<int> dst_pos, Rect<int> src_rect, FlipFlags flags = FlipFlags()) = 0;

    inline void putImage(PainterTexture2D* texture, Point<int> dst_pos, FlipFlags flags = FlipFlags())
    {
        putImage(texture, dst_pos, {{0, 0}, texture->size()}, flags);
    }

    /* Blend multiple colors using multi-component alpha mask. */
    virtual void blendColors(Point<int> dst_pos, const Colors &colors, Image* mask_image, FlipFlags flags = FlipFlags()) = 0;

    virtual void blendColors(Point<int> dst_pos, const Colors &colors, PainterTexture2D* mask_texture, FlipFlags flags = FlipFlags()) = 0;

    virtual void drawWaveform(const Rect<int> &rect, Color color, unsigned char*  waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, Color color, unsigned short* waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, Color color, unsigned int*   waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, Color color, float*          waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, Color color, PainterTexture1D* waveform) = 0;

    virtual void drawPoleZeroPlot(const Rect<int> &rect, PainterTexture1D* tex, int zero_index, int zero_count, int pole_index, int pole_count) = 0;

    virtual void tileImage(PainterTexture2D* texture, const Rect<int> &rect, const Point<int> &shift) = 0;

    virtual void tileImage(Image* image, const Rect<int> &rect, const Point<int> &shift) = 0;

    virtual void beginPaintGroup() = 0;

    virtual void endPaintGroup() = 0;

    virtual void resetPaintGroups() = 0;

    /** Make the changes visible.

        Update window surface. Swap buffers. etc.
        Depending on the window type.
     */
    virtual void repaint(Rect<int>* rects = nullptr, int numrects = 0) = 0;

    virtual void setViewport(const Rect<int> &rect) = 0;

    /* Allways call this before any other paint commands
     * if the window has been resized. */
    virtual void adjustForWindowSize() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP
