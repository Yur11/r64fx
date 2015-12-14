#include "ReconfContext.hpp"
#include <iostream>

using namespace std;

namespace r64fx{

ReconfContext::ReconfContext(int max_rects)
: max_rects(max_rects)
{
    rects = new Rect<int>[max_rects];
}


void ReconfContext::addRect(const Rect<int> &rect)
{
    if(num_rects < max_rects)
    {
        rects[num_rects] = rect;
        num_rects++;
    }
    else
    {
        cerr << "ReconfContext: Too many rects!\n";
    }
}


void ReconfContext::clearRects()
{
    num_rects = 0;
}

}//namespace r64x