#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Color.hpp"
#include "Rect.hpp"
#include "Offset.hpp"
#include "Orientation.hpp"

namespace r64fx{

class Image;
class Window;
class PainterTexture;
class PainterTexture1D;
class PainterTexture2D;


class PainterTextureManager{
public:
    virtual PainterTexture1D* newTexture() = 0;

    virtual PainterTexture1D* newTexture(unsigned char*  data, int length, int component_count) = 0;

    virtual PainterTexture1D* newTexture(unsigned short* data, int length, int component_count) = 0;

    virtual PainterTexture1D* newTexture(unsigned int*   data, int length, int component_count) = 0;

    virtual PainterTexture1D* newTexture(float* data, int length, int component_count) = 0;

    virtual PainterTexture2D* newTexture(Image* image = nullptr) = 0;

    virtual void deleteTexture(PainterTexture1D** texture) = 0;

    virtual void deleteTexture(PainterTexture2D** texture) = 0;
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

    /** Put an image at the given position.

        image  Image to use. Must not be null.
                      For now RGBA only!

        pos    Position to put the image at.
                      Top left corner.
                      Offset and clipping are applied.
     */
    virtual void putImage(Image* image, Point<int> dst_pos) = 0;

    /**  */
    virtual void putImage(PainterTexture2D* texture, Point<int> dst_pos) = 0;

    /* Blend multiple colors using multi-component alpha mask. */
    virtual void blendColors(Point<int> dst_pos, const Colors &colors, Image* mask_image) = 0;

    virtual void blendColors(Point<int> dst_pos, const Colors &colors, PainterTexture2D* mask_texture) = 0;

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned char*  waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned short* waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned int*   waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float*          waveform) = 0;

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, PainterTexture1D* waveform) = 0;

    virtual void beginPaintGroup() = 0;

    virtual void endPaintGroup() = 0;

    virtual void resetPaintGroups() = 0;

    /** Make the changes visible.

        Update window surface. Swap buffers. etc.
        Depending on the window type.
     */
    virtual void repaint(Rect<int>* rects = nullptr, int numrects = 0) = 0;

    /* Allways call this before any other paint commands
     * if the window has been resized. */
    virtual void adjustForWindowSize() = 0;
};


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
    virtual Point<int> position() = 0;

    virtual Size<int> size() = 0;

    virtual void loadImage(Image* teximg) = 0;

    virtual void readImage(Image* teximg) = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP
