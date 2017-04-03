/* To be included in PainterGL.cxx */

namespace r64fx{

class PainterTextureImplGL{
protected:
    PainterImplGL*  m_parent_painter   = nullptr;
    GLuint          m_texture          = 0;
    int             m_component_count  = 0;

    PainterTextureImplGL(PainterImplGL* parent_painter)
    : m_parent_painter(parent_painter)
    {}

    bool formats(int component_count, GLenum &internal_format, GLenum &format)
    {
#ifdef R64FX_DEBUG
        assert(component_count >= 1);
        assert(component_count <= 4);
#endif//R64FX_DEBUG

        if(component_count == 1)
        {
            internal_format = GL_R8;
            format = GL_RED;
        }
        else if(m_component_count == 2)
        {
            internal_format = GL_RG8;
            format = GL_RG;
        }
        else if(component_count == 3)
        {
            internal_format = GL_RGB8;
            format = GL_RGB;
        }
        else if(component_count == 4)
        {
            internal_format = GL_RGBA8;
            format = GL_RGBA;
        }
        else
        {
            return false;
        }

        return true;
    }
};


class PainterTexture1DImplGL
: public PainterTexture1D
, public LinkedList<PainterTexture1DImplGL>::Node
, public PainterTextureImplGL{
    int    m_length      = 0;
    float  m_length_rcp  = 0;
    Type   m_data_type;

public:
    PainterTexture1DImplGL(PainterImplGL* parent_painter)
    : PainterTextureImplGL(parent_painter)
    {}

    Type dataType()
    {
        return m_data_type;
    }

    virtual Painter* parentPainter();

    virtual bool isGood()
    {
        return m_texture != 0;
    }

    virtual int componentCount()
    {
        return m_component_count;
    }

    virtual void free()
    {
        if(!isGood())
            return;

        gl::DeleteTextures(1, &m_texture);
        m_texture = 0;
    }

    inline void bind()
    {
        gl::BindTexture(GL_TEXTURE_1D, m_texture);
    }

    virtual int length()
    {
        return 0;
    }

    virtual float lengthRcp()
    {
        return m_length_rcp;
    }

    template<typename T> void loadData(T* data, int length, int component_count, GLenum type)
    {
#ifdef R64Fx_DEBUG
        assert(data != nullptr);
        assert(length > 0);
        assert(component_count > 0);
#endif//R64FX_DEBUG

        free();

        gl::GenTextures(1, &m_texture);
        gl::BindTexture(GL_TEXTURE_1D, m_texture);
        gl::TexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl::TexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        m_component_count = component_count;
        GLenum internal_format = 0, format = 0;
        formats(m_component_count, internal_format, format);

        gl::TexStorage1D(
            GL_TEXTURE_1D,
            1,
            internal_format,
            length
        );

        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

        gl::TexSubImage1D(
            GL_TEXTURE_1D,
            0,
            0,
            length,
            format,
            type,
            data
        );

        m_length = length;
        m_length_rcp = 1.0f / float(length);
    }

    virtual void load(unsigned char* data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_UNSIGNED_BYTE);
        m_data_type = PainterTexture1D::Type::UnsignedChar;
    }

    virtual void load(unsigned short*  data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_UNSIGNED_SHORT);
        m_data_type = PainterTexture1D::Type::UnsignedShort;
    }

    virtual void load(unsigned int*    data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_UNSIGNED_INT);
        m_data_type = PainterTexture1D::Type::UnsignedInt;
    }

    virtual void load(float* data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_FLOAT);
        m_data_type = PainterTexture1D::Type::Float;
    }
};//PainterTexture1DImplGL


class PainterTexture2DImplGL
: public PainterTexture2D
, public LinkedList<PainterTexture2DImplGL>::Node
, public PainterTextureImplGL {
    Point<int>      m_posision;
    Size<int>       m_size;
    float           m_wrcp             = 1.0f;
    float           m_hrcp             = 1.0f;

public:
    PainterTexture2DImplGL(PainterImplGL* parent_painter)
    : PainterTextureImplGL(parent_painter)
    {

    }

    inline PainterImplGL* painter() const
    {
        return m_parent_painter;
    }

    inline int width() const
    {
        return m_size.width();
    }

    inline int height() const
    {
        return m_size.height();
    }

    inline float wrcp() const
    {
        return m_wrcp;
    }

    inline float hrcp() const
    {
        return m_hrcp;
    }

    inline void bind()
    {
        gl::BindTexture(GL_TEXTURE_2D, m_texture);
    }

    virtual Painter* parentPainter();

    virtual bool isGood()
    {
        return m_texture != 0;
    }

    virtual int componentCount()
    {
        return m_component_count;
    }

    virtual Point<int> position()
    {
        return m_posision;
    }

    virtual Size<int> size()
    {
        return m_size;
    }

    virtual void loadImage(Image* teximg)
    {
#ifdef R64FX_DEBUG
        assert(teximg != nullptr);
        assert(teximg->width() > 0);
        assert(teximg->height() > 0);
        assert(teximg->componentCount() > 0);
        assert(teximg->componentCount() <= 4);
#endif//R64FX_DEBUG

        free();

        gl::GenTextures(1, &m_texture);
        gl::BindTexture(GL_TEXTURE_2D, m_texture);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        m_component_count = teximg->componentCount();
        GLenum internal_format = 0, format = 0;
        formats(m_component_count, internal_format, format);

        gl::TexStorage2D(
            GL_TEXTURE_2D,
            1,
            internal_format,
            teximg->width(), teximg->height()
        );

        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

        gl::TexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            teximg->width(),
            teximg->height(),
            format,
            GL_UNSIGNED_BYTE,
            teximg->data()
        );

        m_size = {teximg->width(), teximg->height()};
        m_wrcp = 1.0f / float(teximg->width());
        m_hrcp = 1.0f / float(teximg->height());
    }

    virtual void readImage(Image* teximg)
    {

    }

    virtual void free()
    {
        if(!isGood())
            return;

        gl::DeleteTextures(1, &m_texture);
        m_texture = 0;
    }
};//PainterTexture2DImplGL

}//namespace r64fx
