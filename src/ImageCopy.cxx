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

#define R64FX_IMGOP_SWITCH_MASK (R64FX_IMGOP_FLIP_MASK | R64FX_IMGOP_TYPE_MASK)

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


template<bool> inline int flipval(int val, int)
{
    return val;
}

template<> inline int flipval<true>(int val, int size)
{
    return size - val - 1;
}


template<bool> inline int secondval(int first, int)
{
    return first;
}

template<> inline int secondval<true>(int, int second)
{
    return second;
}


template<unsigned int Flags> struct CopyFun{
    void operator()(const ImgPos &dst, const ImgRect &src, const ImgCopyFlags flags)
    {
        int srcw = secondval<Flags & R64FX_IMGOP_FLIP_DIAG>(src.rect.width(), src.rect.height());
        int srch = secondval<Flags & R64FX_IMGOP_FLIP_DIAG>(src.rect.height(), src.rect.width());

        RectIntersection<int> isec({0, 0, dst.img->width(), dst.img->height()}, {dst.pos.x(), dst.pos.y(), srcw, srch});

        UnpackPixopChanShuf shuf(flags.bits(), dst.img, src.img);
        for(int y=0; y<isec.height(); y++)
        {
            int fy = flipval<Flags & R64FX_IMGOP_FLIP_VERT>(y, srch);
            int srcy = fy + isec.srcy() + src.rect.y();
            for(int x=0; x<isec.width(); x++)
            {
                int fx = flipval<Flags & R64FX_IMGOP_FLIP_HORI>(x, srcw);
                int srcx = fx + isec.srcx() + src.rect.x();
                auto srcpx = src.img->pixel(
                    secondval<Flags & R64FX_IMGOP_FLIP_DIAG>(srcx, srcy),
                    secondval<Flags & R64FX_IMGOP_FLIP_DIAG>(srcy, srcx)
                );

                auto dstpx = dst.img->pixel(
                    x + isec.dstx(), y + isec.dsty()
                );
                shuf_components<Flags & R64FX_IMGOP_TYPE_MASK>(shuf, dstpx, srcpx);
            }
        }
    }
};


#define R64FX_IMGOP_OPTION(OP) case (OP): { CopyFun<OP> fun; fun(dst, src, flags); } break

void copy(const ImgPos &dst, const ImgRect &src, const ImgCopyFlags flags)
{
#ifdef R64FX_DEBUG
    assert(dst.img != nullptr);
    assert(src.img != nullptr);
#endif//R64FX_DEBUG

    switch(flags.bits() & R64FX_IMGOP_SWITCH_MASK)
    {
        R64FX_IMGOP_OPTION(0x0000);
        R64FX_IMGOP_OPTION(0x0100);
        R64FX_IMGOP_OPTION(0x0200);
        R64FX_IMGOP_OPTION(0x0300);
        R64FX_IMGOP_OPTION(0x0400);
        R64FX_IMGOP_OPTION(0x0500);
        R64FX_IMGOP_OPTION(0x0600);
        R64FX_IMGOP_OPTION(0x0700);
        R64FX_IMGOP_OPTION(0x0800);
        R64FX_IMGOP_OPTION(0x0900);
        R64FX_IMGOP_OPTION(0x0A00);
        R64FX_IMGOP_OPTION(0x0B00);
        R64FX_IMGOP_OPTION(0x0C00);
        R64FX_IMGOP_OPTION(0x0D00);
        R64FX_IMGOP_OPTION(0x0E00);
        R64FX_IMGOP_OPTION(0x0F00);

        R64FX_IMGOP_OPTION(0x1000);
        R64FX_IMGOP_OPTION(0x1100);
        R64FX_IMGOP_OPTION(0x1200);
        R64FX_IMGOP_OPTION(0x1300);
        R64FX_IMGOP_OPTION(0x1400);
        R64FX_IMGOP_OPTION(0x1500);
        R64FX_IMGOP_OPTION(0x1600);
        R64FX_IMGOP_OPTION(0x1700);
        R64FX_IMGOP_OPTION(0x1800);
        R64FX_IMGOP_OPTION(0x1900);
        R64FX_IMGOP_OPTION(0x1A00);
        R64FX_IMGOP_OPTION(0x1B00);
        R64FX_IMGOP_OPTION(0x1C00);
        R64FX_IMGOP_OPTION(0x1D00);
        R64FX_IMGOP_OPTION(0x1E00);
        R64FX_IMGOP_OPTION(0x1F00);

        R64FX_IMGOP_OPTION(0x2000);
        R64FX_IMGOP_OPTION(0x2100);
        R64FX_IMGOP_OPTION(0x2200);
        R64FX_IMGOP_OPTION(0x2300);
        R64FX_IMGOP_OPTION(0x2400);
        R64FX_IMGOP_OPTION(0x2500);
        R64FX_IMGOP_OPTION(0x2600);
        R64FX_IMGOP_OPTION(0x2700);
        R64FX_IMGOP_OPTION(0x2800);
        R64FX_IMGOP_OPTION(0x2900);
        R64FX_IMGOP_OPTION(0x2A00);
        R64FX_IMGOP_OPTION(0x2B00);
        R64FX_IMGOP_OPTION(0x2C00);
        R64FX_IMGOP_OPTION(0x2D00);
        R64FX_IMGOP_OPTION(0x2E00);
        R64FX_IMGOP_OPTION(0x2F00);

        R64FX_IMGOP_OPTION(0x3000);
        R64FX_IMGOP_OPTION(0x3100);
        R64FX_IMGOP_OPTION(0x3200);
        R64FX_IMGOP_OPTION(0x3300);
        R64FX_IMGOP_OPTION(0x3400);
        R64FX_IMGOP_OPTION(0x3500);
        R64FX_IMGOP_OPTION(0x3600);
        R64FX_IMGOP_OPTION(0x3700);
        R64FX_IMGOP_OPTION(0x3800);
        R64FX_IMGOP_OPTION(0x3900);
        R64FX_IMGOP_OPTION(0x3A00);
        R64FX_IMGOP_OPTION(0x3B00);
        R64FX_IMGOP_OPTION(0x3C00);
        R64FX_IMGOP_OPTION(0x3D00);
        R64FX_IMGOP_OPTION(0x3E00);
        R64FX_IMGOP_OPTION(0x3F00);

        default:
        {
            cerr << "Bad ImgCopyFlags: 0x" << std::hex << flags.bits() << "\n";
            break;
        }
    }
}


template<unsigned int ImgCopyType> void perform_copy(const ImgRect &dst, Transformation2D<float> transform, Image* src, const ImgCopyFlags flags)
{
#ifdef R64FX_DEBUG
    assert(dst.img != nullptr);
    assert(src != nullptr);
#endif//R64FX_DEBUG
    UnpackPixopChanShuf shuf(flags.bits(), dst.img, src);

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


void copy(const ImgRect &dst, Transformation2D<float> t, Image* src, const ImgCopyFlags flags)
{
    switch(flags.bits() & R64FX_IMGOP_TYPE_MASK)
    {
#define SWITCH_PIXOP(OP) case OP: perform_copy<OP>(dst, t, src, flags); break
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
