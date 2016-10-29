#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Rect.hpp"
#include "Offset.hpp"
#include "Orientation.hpp"

namespace r64fx{

class Image;
class Window;
class PainterTexture;


class PainterTextureManager{
public:
    /* Generate a new texture. Optionally load the texture image. */
    virtual PainterTexture* newTexture(Image* img = nullptr) = 0;
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

    /** Fill a rectangle with the given color.

        rect   Rectangle coords. to fill.
               Offset and clipping are applied.
               to the rectange before it's used.

        color  Color to use.
     */
    virtual void fillRect(const Rect<int> &rect, unsigned char* color) = 0;

//     /** Put an image at the given position.
// 
//         image  Image to use. Must not be null.
//                       For now RGBA only!
// 
//         pos    Position to put the image at.
//                       Top left corner.
//                       Offset and clipping are applied.
//      */
//     virtual void putImage(Image* img, Point<int> pos = {0, 0}) = 0;
// 
//     /* Same as putImage() but with alpha blend. */
//     virtual void blendImage(Image* img, Point<int> pos = {0, 0}) = 0;
// 
//     /* Blend multiple colors using multi-component alpha mask. */
//     virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask) = 0;


    
    
    virtual void drawTexture(PainterTexture* texture, Point<int> dst_pos, bool blend_alpha = false) = 0;
    
    virtual void blendColors(Point<int> pos, unsigned char** colors, PainterTexture* mask_texture) = 0;
    
    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float* waveform, float gain) = 0;
    
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
    virtual bool isGood() = 0;
    
    virtual Painter* parentPainter() = 0;
    
    virtual Rect<int> rect() = 0;
    
    virtual int componentCount() = 0;
    
    virtual void loadImage(Image* teximg) = 0;
    
    virtual void readImage(Image* teximg) = 0;
    
    virtual void free() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP
