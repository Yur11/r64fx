/* To be included in PainterGL.cxx */

namespace r64fx{

namespace{

int format(int component_count)
{
#ifdef R64FX_DEBUG
    assert(component_count >= 1);
    assert(component_count <= 4);
#endif//R64FX_DEBUG

    switch(component_count)
    {
        case 1:
            return GL_RED;

        case 2:
            return GL_RG;

        case 3:
            return GL_RGB;

        case 4:
            return GL_RGBA;

        default:
            return 0;
    }
}

int internal_format(int component_count)
{
#ifdef R64FX_DEBUG
    assert(component_count >= 1);
    assert(component_count <= 4);
#endif//R64FX_DEBUG

    switch(component_count)
    {
        case 1:
            return GL_R8;

        case 2:
            return GL_RG8;

        case 3:
            return GL_RGB8;

        case 4:
            return GL_RGBA8;

        default:
            return 0;
    }
}

}//namespace


class PainterTextureImplGL{
protected:
    PainterImplGL*  m_parent_painter   = nullptr;
    GLuint          m_texture          = 0;
    int             m_component_count  = 0;

    PainterTextureImplGL(PainterImplGL* parent_painter)
    : m_parent_painter(parent_painter)
    {}
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

        gl::TexStorage1D(
            GL_TEXTURE_1D,
            1,
            internal_format(component_count),
            length
        );

        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

        gl::TexSubImage1D(
            GL_TEXTURE_1D,
            0,
            0,
            length,
            format(component_count),
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
    Size<int>       m_size;

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

    virtual Size<int> size()
    {
        return m_size;
    }

    virtual void allocStorage(Size<int> size, int component_count)
    {
        free();

        gl::GenTextures(1, &m_texture);
        gl::BindTexture(GL_TEXTURE_2D, m_texture);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        gl::TexStorage2D(
            GL_TEXTURE_2D,
            1,
            internal_format(component_count),
            size.width(), size.height()
        );

        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

        m_size = size;
        m_component_count = component_count;
    }

    virtual void loadSubImage(const Point<int> &dstpos, const ImgRect &src)
    {
#ifdef R64FX_DEBUG
        assert(src.rect.left()    >=  0);
        assert(src.rect.top()     >=  0);
        assert(src.rect.right()   <=  m_size.width());
        assert(src.rect.bottom()  <=  m_size.height());
        assert(src.img->componentCount() == m_component_count);
#endif//R64FX_DEBUG

        gl::TexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            src.img->width(),
            src.img->height(),
            format(src.img->componentCount()),
            GL_UNSIGNED_BYTE,
            src.img->data()
        );
    }

    virtual void loadImage(Image* teximg)
    {
#ifdef R64FX_DEBUG
        assert(teximg != nullptr);
        assert(teximg->width()           >   0);
        assert(teximg->height()          >   0);
        assert(teximg->componentCount()  >   0);
        assert(teximg->componentCount()  <=  4);
#endif//R64FX_DEBUG

        PainterTexture2DImplGL::allocStorage({teximg->width(), teximg->height()}, teximg->componentCount());
        PainterTexture2DImplGL::loadSubImage({0, 0}, teximg);
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
