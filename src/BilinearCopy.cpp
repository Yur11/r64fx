#include "BilinearCopy.hpp"
#include "Point.hpp"
#include <algorithm>

using namespace std;

namespace r64fx{
    
void BilinearCopy::operator()(Image &src, Image &dst, Transform2D<float> transform)
{
    int r = src.r();
    int g = src.g();
    int b = src.b();
    int a = src.a();
    int C = min(src.channelCount(), dst.channelCount());
    
    float fill[5];
    fill[r] = fillr;
    fill[g] = fillg;
    fill[b] = fillb;
    fill[a] = filla;

    for(int y=0; y<dst.height(); y++)
    {
        for(int x=0; x<dst.width(); x++)
        {
            Point<float> p(x, y);
            transform(p);
                
            float x1 = floor(p.x);
            float y1 = floor(p.y);
            float x2 = x1 + 1;
            float y2 = y1 + 1;

            float fracx = x2 - p.x;
            float fracy = y2 - p.y;
            for(int c=0; c<C; c++)
            {
                float p11 = fill[c];
                float p12 = fill[c];
                float p21 = fill[c];
                float p22 = fill[c];
                
                if(x1 >=0 && x1 < src.width() && y1 >=0 && y1 < src.height())
                {
                    p11 = src(x1, y1)[c];
                }
                
                if(x1 >=0 && x1 < src.width() && y2 >=0 && y2 < src.height())
                {
                    p12 = src(x1, y2)[c];
                }
                
                if(x2 >=0 && x2 < src.width() && y1 >=0 && y1 < src.height())
                {
                    p21 = src(x2, y1)[c];
                }
                
                if(x2 >=0 && x2 < src.width() && y2 >=0 && y2 < src.height())
                {
                    p22 = src(x2, y2)[c];
                }
                
                float val =
                    p22 * (1-fracx) * (1-fracy) +
                    p12 * fracx     * (1-fracy) +
                    p21 * (1-fracx) * fracy     +
                    p11 * fracx     * fracy;
                dst(x, y)[c] = val;
            }
        }
    }
}
    
}//namespace r64fx