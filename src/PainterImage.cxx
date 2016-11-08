/* To be included in Painter.cpp */

namespace r64fx{

struct PainterImpl : public Painter{
    Window*     window    = nullptr;

    Rect<int> current_clip_rect;

    PainterImpl(Window* window)
    : window(window)
    {

    }

    virtual ~PainterImpl()
    {

    }

    virtual void setClipRect(Rect<int> rect)
    {
        current_clip_rect = rect;
    }

    virtual void setClipRectAtCurrentOffset(Size<int> size)
    {
        current_clip_rect = {offset(), size};
    }

    virtual Rect<int> clipRect()
    {
        return current_clip_rect;
    }

    /** @brief Clip a rectangle with the current_clip_rect. */
    Rect<int> clip(Rect<int> rect)
    {
        return intersection(
            current_clip_rect,
            rect
        );
    }
};//PainterImpl


class PainterImplImage;


class PainterTexture1DImplImage : public PainterTexture1D, public LinkedList<PainterTexture1DImplImage>::Node{
    PainterImplImage* m_parent_painter   = nullptr;
    
    union Data{
        unsigned char*  uc;
        unsigned short* us;
        unsigned int*   ui;
        float*          f;      
    } m_data;
    
    Type m_data_type;
    
    int m_length           = 0;
    int m_component_count  = 0;

public:
    PainterTexture1DImplImage(PainterImplImage* parent_painter)
    : m_parent_painter(parent_painter)
    {

    }
    
    virtual Type dataType()
    {
#ifdef R64FX_DEBUG
        assert(m_data_type == Type::UnsignedChar);
#endif//R64FX_DEBUG
        return m_data_type;
    }
    
    inline unsigned char* dataUnsignedChar() const
    {
#ifdef R64Fx_DEBUG
        assert(m_data_type == Type::UnsignedShort);
#endif//R64FX_DEBUG
        return m_data.uc;
    }
    
    inline unsigned short* dataUnsignedShort() const
    {
#ifdef R64FX_DEBUG
        assert(m_data_type == Type::UnsignedInt);
#endif//R64FX_DEBUG
        return m_data.us;
    }
    
    inline unsigned int* dataUnsignedInt() const
    {
#ifdef R64FX_DEBUG
        assert(m_data_type == Type::Float);
#endif//R64FX_DEBUG
        return m_data.ui;
    }
    
    inline float* dataFloat() const
    {
        return m_data.f;
    }

    virtual Painter* parentPainter();

    virtual int length()
    {
        return m_length;
    }

    virtual bool isGood()
    {
        return (m_data.f != nullptr && m_length > 0 && m_component_count > 0);
    }

    virtual int componentCount()
    {
        return m_component_count;
    }

    template<typename T> inline void loadData(T* &own_data, T* data, int length, int component_count)
    {
        free();

        m_length = length;
        m_component_count = component_count;

        int nbytes = length * component_count * sizeof(T);
        own_data = new T[nbytes];
        memcpy(own_data, data, nbytes);
    }
    
    virtual void load(unsigned char* data, int length, int component_count)
    {
        loadData<unsigned char>(m_data.uc, data, length, component_count);
        m_data_type = Type::UnsignedChar;
    }

    virtual void load(unsigned short* data, int length, int component_count)
    {
        loadData<unsigned short>(m_data.us, data, length, component_count);
        m_data_type = Type::UnsignedShort;
    }

    virtual void load(unsigned int* data, int length, int component_count)
    {
        loadData<unsigned int>(m_data.ui, data, length, component_count);
        m_data_type = Type::UnsignedInt;
    }

    virtual void load(float* data, int length, int component_count)
    {
        loadData<float>(m_data.f, data, length, component_count);
        m_data_type = Type::Float;
    }

    virtual void free()
    {
        if(m_data.f)
        {
            if(m_data_type == Type::UnsignedChar)
                delete[] m_data.uc;
            else if(m_data_type == Type::UnsignedShort)
                delete[] m_data.us;
            else if(m_data_type == Type::UnsignedInt)
                delete[] m_data.ui;
            else
                delete[] m_data.f;
            m_data.f = nullptr;
            m_length = 0;
            m_component_count = 0;
        }
    }
};


class PainterTexture2DImplImage : public PainterTexture2D, public LinkedList<PainterTexture2DImplImage>::Node{
    PainterImplImage* m_parent_painter;
    Point<int> m_pos;
    Image m_img;
    
public:
    PainterTexture2DImplImage(PainterImplImage* parent_painter)
    : m_parent_painter(parent_painter)
    {

    }

    virtual ~PainterTexture2DImplImage()
    {
        
    }
    
    inline PainterImplImage* painter() const
    {
        return m_parent_painter;
    }
    
    inline int width() const
    {
        return m_img.width();
    }
    
    inline int height() const
    {
        return m_img.height();
    }
    
    inline Image* image()
    {
        return &m_img;
    }
    
    virtual Painter* parentPainter();
    
    virtual bool isGood()
    {
        return m_img.isGood();
    }

    virtual Point<int> position()
    {
        return m_pos;
    }
    
    virtual Size<int> size()
    {
        return {m_img.width(), m_img.height()};
    }
    
    virtual int componentCount()
    {
        return m_img.componentCount();
    }
    
    virtual void loadImage(Image* teximg)
    {
        m_img.load(teximg->width(), teximg->height(), teximg->componentCount(), teximg->data(), true);
    }
    
    virtual void readImage(Image* teximg)
    {
        teximg->load(m_img.width(), m_img.height(), m_img.componentCount(), m_img.data(), true);
    }
    
    virtual void free()
    {
        m_img.free();
    }
};


struct PainterImplImage : public PainterImpl{
    LinkedList<PainterTexture1DImplImage> m_1d_textures;
    LinkedList<PainterTexture2DImplImage> m_2d_textures;

    PainterImplImage(Window* window)
    : PainterImpl(window)
    {

    }

    virtual ~PainterImplImage()
    {

    }

    virtual void clear(unsigned char* color)
    {
        fill(window->image(), color, {0, 0, window->width(), window->height()});
    }

    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        auto intersection_rect = clip(rect + offset());
        if(intersection_rect.width() > 0 && intersection_rect.height() > 0)
        {
            fill(window->image(), color, intersection_rect);
        }
    }

    virtual void putImage(Image* img, Point<int> dst_pos)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), img->width(), img->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            implant_alpha(
                window->image(),
                intersection.dstOffset() + current_clip_rect.position(),
                intersection.size(),
                intersection.srcOffset(),
                img
            );
        }
    }

    virtual void putImage(PainterTexture2D* texture, Point<int> dst_pos)
    {
#ifdef R64FX_DEBUG
        assert(texture != nullptr);
        assert(texture->parentPainter() == this);
#endif//R64FX_DEBUG
        
        auto texture_impl = static_cast<PainterTexture2DImplImage*>(texture);
        PainterImplImage::putImage(texture_impl->image(), dst_pos);
    }

    virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {pos.x() + offsetX(), pos.y() + offsetY(), mask->width(), mask->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            blend(
                window->image(),
                intersection.dstOffset() + current_clip_rect.position(),
                intersection.size(),
                intersection.srcOffset(),
                colors, mask
            );
        }
    }

    virtual void blendColors(Point<int> pos, unsigned char** colors, PainterTexture2D* mask)
    {
#ifdef R64FX_DEBUG
        assert(mask != nullptr);
        assert(mask->parentPainter() == this);
#endif//R64FX_DEBUG

        auto mask_texture_impl = static_cast<PainterTexture2DImplImage*>(mask);
        PainterImplImage::blendColors(pos, colors, mask_texture_impl->image());
    }

    template<typename T> void drawWaveform(const Rect<int> &rect, unsigned char* color, T* waveform)
    {
        RectIntersection<int> intersection(
            current_clip_rect, rect + offset()
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            draw_waveform(
                window->image(),
                color,
                waveform + intersection.srcOffset().x() * 2,
                Rect<int>(intersection.dstOffset(), intersection.size())
            );
        }
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned char* waveform)
    {
        drawWaveform<unsigned char>(rect, color, waveform);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned short* waveform)
    {
        drawWaveform<unsigned short>(rect, color, waveform);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned int* waveform)
    {
        drawWaveform<unsigned int>(rect, color, waveform);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float* waveform)
    {
        drawWaveform<float>(rect, color, waveform);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, PainterTexture1D* waveform)
    {
#ifdef R64FX_DEBUG
        assert(waveform->length() == rect.width());
#endif//R64FX_DEBUG
        
        auto waveform_texture_impl = static_cast<PainterTexture1DImplImage*>(waveform);

        if(waveform->dataType() == PainterTexture1D::Type::UnsignedChar)
        {
            PainterImplImage::drawWaveform(rect, color, waveform_texture_impl->dataUnsignedChar());
        }
        else if(waveform->dataType() == PainterTexture1D::Type::UnsignedShort)
        {
            PainterImplImage::drawWaveform(rect, color, waveform_texture_impl->dataUnsignedShort());
        }
        else if(waveform->dataType() == PainterTexture1D::Type::UnsignedInt)
        {
            PainterImplImage::drawWaveform(rect, color, waveform_texture_impl->dataUnsignedInt());
        }
        else if(waveform->dataType() == PainterTexture1D::Type::Float)
        {
            PainterImplImage::drawWaveform(rect, color, waveform_texture_impl->dataFloat());
        }
}

    virtual PainterTexture1D* newTexture()
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned char* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned short* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned int*   data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(float* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture2D* newTexture(Image* image = nullptr)
    {
        auto texture_impl = new PainterTexture2DImplImage(this);
        m_2d_textures.append(texture_impl);
        if(image)
        {
            texture_impl->loadImage(image);
        }
        return texture_impl;
    }

    virtual void deleteTexture(PainterTexture1D* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture1DImplImage*>(texture);
            m_1d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
        }
    }

    virtual void deleteTexture(PainterTexture2D* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture2DImplImage*>(texture);
            m_2d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
        }
    }

    virtual void repaint(Rect<int>* rects, int numrects)
    {
        for(int i=0; i<numrects; i++)
        {
            rects[i] = clip(rects[i]);
        }
        window->repaint(rects, numrects);
    }

    virtual void adjustForWindowSize()
    {
        setClipRect({0, 0, window->width(), window->height()});
        setOffset({0, 0});
    }
};//PainterImplImage


Painter* PainterTexture1DImplImage::parentPainter()
{
    return m_parent_painter;
}


Painter* PainterTexture2DImplImage::parentPainter()
{
    return m_parent_painter;
}

}//namespace r64fx
