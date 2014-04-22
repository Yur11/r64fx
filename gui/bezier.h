#ifndef R64FX_GUI_BEZIER_H
#define R64FX_GUI_BEZIER_H

#include <vector>
#include "Point.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

namespace r64fx{

/** @brief Calculate coordinates of a point that marks a position distance on a segment. 
 
    Points a and b describe the segment.
    
    @param position - Must be within [ 0.0 .. 1.0 ] range.
    If the position is set to 0.0, then the function returns the value of a.
    If the position is set to 1.0, then the function returns the value of b.
    If the position is set to 0.5, then the function returns the position halfway between a and b.
    You get the idea.
    
    This is the atomic algorithm used to construct bezier curves, but do use it for anything else you may need.
 */
Point<float> point_on_a_segment(Point<float> a, Point<float> b, float position);


/** @brief Calculate coordinates of a point that resides on a bezier curve. */
Point<float> bezier_point(const std::vector<Point<float>> &in, float position);


/** @brief Calculate N equdistant points that lie on a bezier curve.
 
    @param in - A set of input points of type Point< float >. End points and control points.
    
    @param out - A set of output points of type OutT. 
    Prefill this vector with a N points that can have any value.
    The algorithm will simply update their values as it goes.
    An OutT(Point< float >) ctor. must be available.
 */
template<typename OutT>
void bezier_points(const std::vector<Point<float>> &in, std::vector<OutT> &out)
{
#ifdef DEBUG
    assert(in.size() >= 2);
    assert(out.size() >= 1);
#endif//DEBUG

    out.back() = in.back();
    out.front() = in.front();
    
    float step = 1.0 / float(out.size()-1);
    float position = 0;

    for(int i=0; i<(int)out.size(); i++)
    {
        out[i] = bezier_point(in, position);
        position += step;
    }
}

}//namespace r64fx

#endif//R64FX_GUI_BEZIER_H