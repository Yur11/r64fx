/* To be included in ImageUtils.cpp */

#define R64FX_IMGOP_SHUF_MASK           0x000000FF

#define R64FX_IMGOP_FLIP_MASK           0x00003800
#define R64FX_IMGOP_FLIP_VERT           0x00000800
#define R64FX_IMGOP_FLIP_HORI           0x00001000
#define R64FX_IMGOP_FLIP_DIAG           0x00002000

#define R64FX_IMGOP_TYPE_MASK           0x00000700
#define R64FX_IMGOP_REPLACE             0x00000000
#define R64FX_IMGOP_ADD                 0x00000100
#define R64FX_IMGOP_SUB                 0x00000200
#define R64FX_IMGOP_MUL                 0x00000300
#define R64FX_IMGOP_MIN                 0x00000400
#define R64FX_IMGOP_MAX                 0x00000500
#define R64FX_IMGOP_SRC_ALPHA           0x00000600
#define R64FX_IMGOP_SRC_ALPHA_ACCURATE  0x00000700

namespace r64fx{

ImgCopyFlags ImgCopyFlipVert()
{
    return R64FX_IMGOP_FLIP_VERT;
}

ImgCopyFlags ImgCopyFlipHori()
{
    return R64FX_IMGOP_FLIP_HORI;
}

ImgCopyFlags ImgCopyFlipDiag()
{
    return R64FX_IMGOP_FLIP_DIAG;
}


ImgCopyFlags ChanShuf(int dstc, int ndstc, int srcc, int nsrcc)
{
#ifdef R64FX_DEBUG
    assert(dstc >= 0);
    assert(dstc <= 3);
    assert(srcc >= 0);
    assert(srcc <= 3);
    assert(dstc + ndstc <= 4);
    assert(srcc + nsrcc <= 4);
    if(ndstc > 1)
    {
        assert(nsrcc == ndstc);
    }
#endif//R64FX_DEBUG
    return  dstc | (ndstc << 2) | (srcc << 4) | (nsrcc << 6);
}

struct UnpackPixopChanShuf{
    short dstc = 0, ndstc = 0, srcc = 0, nsrcc = 0;

    UnpackPixopChanShuf(unsigned int bits, Image* dst, Image* src)
    {
        bits &= R64FX_IMGOP_SHUF_MASK;
        if(bits)
        {
            dstc   = (bits & 3);
            ndstc  = (bits & (3 << 2)) >> 2;
            srcc   = (bits & (3 << 4)) >> 4;
            nsrcc  = (bits & (3 << 6)) >> 6;
        }
        else
        {
            ndstc = dst->componentCount();
            nsrcc = src->componentCount();
        }
    }
};


ImgCopyFlags ImgCopyReplace()
{
    return R64FX_IMGOP_REPLACE;
}

ImgCopyFlags ImgCopyAdd()
{
    return R64FX_IMGOP_ADD;
}

ImgCopyFlags ImgCopySub()
{
    return R64FX_IMGOP_SUB;
}

ImgCopyFlags ImgCopyMul()
{
    return R64FX_IMGOP_MUL;
}

ImgCopyFlags ImgCopyMin()
{
    return R64FX_IMGOP_MIN;
}

ImgCopyFlags ImgCopyMax()
{
    return R64FX_IMGOP_MAX;
}

ImgCopyFlags ImgCopyBlendAlpha()
{
    return R64FX_IMGOP_SRC_ALPHA;
}

ImgCopyFlags ImgCopyBlendAlphaAccurate()
{
    return R64FX_IMGOP_SRC_ALPHA_ACCURATE;
}


template<unsigned int ImgCopyType> inline unsigned char src_dst_op(unsigned char dst, unsigned char src)
{
#ifdef R64FX_DEBUG
    cerr << "Warning: Undefined ImgCopyType '" << ImgCopyType << "' in src_dst_op() !\n";
#endif//R64FX_DEBUG
    return 0;
}

template<> inline unsigned char src_dst_op<R64FX_IMGOP_REPLACE>(unsigned char dst, unsigned char src)
{
    return src;
}

template<> inline unsigned char src_dst_op<R64FX_IMGOP_ADD>(unsigned char dst, unsigned char src)
{
    int val = int(dst) + int(src);
    return val <= 255 ? val : 255;
}

template<> inline unsigned char src_dst_op<R64FX_IMGOP_SUB>(unsigned char dst, unsigned char src)
{
    int val = int(dst) - int(src);
    return val >= 0 ? val : 0;
}

template<> inline unsigned char src_dst_op<R64FX_IMGOP_MUL>(unsigned char dst, unsigned char src)
{
    return (unsigned char)((float(dst) * rcp255) * (float(src) * rcp255) * 255.0f);
}

template<> inline unsigned char src_dst_op<R64FX_IMGOP_MIN>(unsigned char dst, unsigned char src)
{
    return dst < src ? dst : src;
}

template<> inline unsigned char src_dst_op<R64FX_IMGOP_MAX>(unsigned char dst, unsigned char src)
{
    return dst > src ? dst : src;
}


template<unsigned int ImgCopyType> unsigned char src_dst_op_alpha(unsigned char c1, float f1, unsigned char c2, float f2)
{
#ifdef R64FX_DEBUG
    cerr << "Warning: Undefined ImgCopyType '" << ImgCopyType << "' in src_dst_op_alpha!\n";
#endif//R64FX_DEBUG
    return 0;
}


template<> unsigned char src_dst_op_alpha<R64FX_IMGOP_SRC_ALPHA>(unsigned char c1, float f1, unsigned char c2, float f2)
{
    return mix_colors(c1, f1, c2, f2);
}

template<> unsigned char src_dst_op_alpha<R64FX_IMGOP_SRC_ALPHA_ACCURATE>(unsigned char c1, float f1, unsigned char c2, float f2)
{
    return mix_colors_accurate(c1, f1, c2, f2);
}


template<unsigned int ImgCopyType> inline void shuf_components(UnpackPixopChanShuf shuf, unsigned char* dstpx, unsigned char* srcpx)
{
    for(auto c=0; c<shuf.ndstc; c++)
    {
        auto &dstref = dstpx[shuf.dstc + c];
        auto &srcref = srcpx[shuf.srcc + (shuf.nsrcc == 1 ? 0 : c)];
        dstref = src_dst_op<ImgCopyType>(dstref, srcref);
    }
}


template<unsigned int ImgCopyType> inline void shuf_components_alpha(unsigned char* dstpx, unsigned char* srcpx, int ndstc, int nsrcc)
{
    switch(nsrcc)
    {
        case 1:
        {
            for(int c=0; c<ndstc - (1 - (ndstc & 1)); c++)
            {
                dstpx[c] = srcpx[0];
            }
            break;
        }

        case 2:
        {
            for(int c=0; c<ndstc - (1 - (ndstc & 1)); c++)
            {
                float alpha            = float(      srcpx[1]) * rcp255;
                float one_minus_alpha  = float(255 - srcpx[1]) * rcp255;
                dstpx[c] = src_dst_op_alpha<ImgCopyType>(dstpx[c], alpha, srcpx[0], one_minus_alpha);
            }
            break;
        }

        case 3:
        {
            for(int c=0; c<3; c++)
            {
                dstpx[c] = srcpx[c];
            }
            break;
        }

        case 4:
        {
            float alpha            = float(      srcpx[3]) * rcp255;
            float one_minus_alpha  = float(255 - srcpx[3]) * rcp255;
            for(int c=0; c<3; c++)
            {
                dstpx[c] = src_dst_op_alpha<ImgCopyType>(dstpx[c], alpha, srcpx[c], one_minus_alpha);
            }
            break;
        }
    }
}


template<> inline void shuf_components<R64FX_IMGOP_SRC_ALPHA>(UnpackPixopChanShuf shuf, unsigned char* dstpx, unsigned char* srcpx)
{
    shuf_components_alpha<R64FX_IMGOP_SRC_ALPHA>(dstpx, srcpx, shuf.ndstc, shuf.nsrcc);
}


template<> inline void shuf_components<R64FX_IMGOP_SRC_ALPHA_ACCURATE>(UnpackPixopChanShuf shuf, unsigned char* dstpx, unsigned char* srcpx)
{
    shuf_components_alpha<R64FX_IMGOP_SRC_ALPHA_ACCURATE>(dstpx, srcpx, shuf.ndstc, shuf.nsrcc);
}


template<unsigned int ImgCopyType> inline void shuf_components_bilinear(
    UnpackPixopChanShuf shuf,
    unsigned char* dstpx, unsigned char* p11, unsigned char* p12, unsigned char* p21, unsigned char* p22,
    double fracx, double fracy
)
{
    int srcinc = (shuf.nsrcc == 1 ? 0 : 1);
    int srci = shuf.srcc;
    for(int dsti=shuf.dstc; dsti<(shuf.dstc+shuf.ndstc); dsti++)
    {
        double val =
            double(p22[srci]) * (1.0-fracx)  * (1.0-fracy) +
            double(p12[srci]) * fracx        * (1.0-fracy) +
            double(p21[srci]) * (1.0-fracx)  * fracy       +
            double(p11[srci]) * fracx        * fracy;

        dstpx[dsti] = src_dst_op<ImgCopyType>(dstpx[dsti], (unsigned char)(val));
        srci += srcinc;
    }
}


template<unsigned int ImgCopyType> struct CopyFun{
    void operator()(Image* dst, Point<int> dstpos, const ImgRect &src, const ImgCopyFlags pixop)
    {
#ifdef R64FX_DEBUG
        assert(dst != nullptr);
        assert(src.img != nullptr);
#endif//R64FX_DEBUG

        RectIntersection<int> src_isec({0, 0, src.img->width(), src.img->height()}, src.rect);
        RectIntersection<int> dst_isec({0, 0, dst->width(), dst->height()}, {dstpos.x(), dstpos.y(), src_isec.width(), src_isec.height()});

        UnpackPixopChanShuf shuf(pixop.bits(), dst, src.img);
        for(int y=0; y<dst_isec.height(); y++)
        {
            for(int x=0; x<dst_isec.width(); x++)
            {
                auto dstpx = dst->pixel     (x + dst_isec.dstx(),                   y + dst_isec.dsty());
                auto srcpx = src.img->pixel (x + dst_isec.srcx() + src_isec.dstx(), y + dst_isec.srcy() + src_isec.dsty());
                shuf_components<ImgCopyType>(shuf, dstpx, srcpx);
            }
        }
    }
};


// struct CopyFun<R64FX_IMGOP_FLIP_VERT>{
//     void operator()(Image* dst, Point<int> dstpos, const ImgRect &src, const ImgCopyFlags pixop)
//     {
//         
//     }
// };


void copy(Image* dst, Point<int> dstpos, const ImgRect &src, const ImgCopyFlags pixop)
{
    switch(pixop.bits() & R64FX_IMGOP_TYPE_MASK)
    {
#define SWITCH_PIXOP(OP) case OP: { CopyFun<OP> fun; fun(dst, dstpos, src, pixop); } break
        SWITCH_PIXOP (R64FX_IMGOP_REPLACE);
        SWITCH_PIXOP (R64FX_IMGOP_ADD);
        SWITCH_PIXOP (R64FX_IMGOP_SUB);
        SWITCH_PIXOP (R64FX_IMGOP_MUL);
        SWITCH_PIXOP (R64FX_IMGOP_MIN);
        SWITCH_PIXOP (R64FX_IMGOP_MAX);
        SWITCH_PIXOP (R64FX_IMGOP_SRC_ALPHA);
        SWITCH_PIXOP (R64FX_IMGOP_SRC_ALPHA_ACCURATE);
        default: break;
#undef SWITCH_PIXOP
    }
}


template<unsigned int ImgCopyType> void perform_copy(const ImgRect &dst, Transformation2D<float> transform, Image* src, const ImgCopyFlags pixop)
{
#ifdef R64FX_DEBUG
    assert(dst.img != nullptr);
    assert(src != nullptr);
#endif//R64FX_DEBUG
    UnpackPixopChanShuf shuf(pixop.bits(), dst.img, src);

    for(int y=dst.rect.top(); y<dst.rect.bottom(); y++)
    {
        for(int x=dst.rect.left(); x<dst.rect.right(); x++)
        {
            auto dstpx = dst.img->pixel(x, y);
            auto p11 = dstpx;
            auto p12 = dstpx;
            auto p21 = dstpx;
            auto p22 = dstpx;

            Point<float> p(x, y);
            transform(p);

            float x1 = floor(p.x());
            float x2 = x1 + 1;

            float y1 = floor(p.y());
            float y2 = y1 + 1;

            float fracx = x2 - p.x();
            float fracy = y2 - p.y();

            if(x1 >=0 && x1 < src->width() && y1 >=0 && y1 < src->height())
            {
                p11 = src->pixel(x1, y1);
            }

            if(x1 >=0 && x1 < src->width() && y2 >=0 && y2 < src->height())
            {
                p12 = src->pixel(x1, y2);
            }

            if(x2 >=0 && x2 < src->width() && y1 >=0 && y1 < src->height())
            {
                p21 = src->pixel(x2, y1);
            }

            if(x2 >=0 && x2 < src->width() && y2 >=0 && y2 < src->height())
            {
                p22 = src->pixel(x2, y2);
            }

            shuf_components_bilinear<ImgCopyType>(shuf, dstpx, p11, p12, p21, p22, fracx, fracy);
        }
    }
}


void copy(const ImgRect &dst, Transformation2D<float> t, Image* src, const ImgCopyFlags pixop)
{
    switch(pixop.bits() & R64FX_IMGOP_TYPE_MASK)
    {
#define SWITCH_PIXOP(OP) case OP: perform_copy<OP>(dst, t, src, pixop); break
        SWITCH_PIXOP (R64FX_IMGOP_REPLACE);
        SWITCH_PIXOP (R64FX_IMGOP_ADD);
        SWITCH_PIXOP (R64FX_IMGOP_SUB);
        SWITCH_PIXOP (R64FX_IMGOP_MUL);
        SWITCH_PIXOP (R64FX_IMGOP_MIN);
        SWITCH_PIXOP (R64FX_IMGOP_MAX);
        default: break;
#undef SWITCH_PIXOP
    }
}

}//namespace r64fx