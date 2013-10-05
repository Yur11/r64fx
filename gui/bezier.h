#ifndef R64FX_GUI_BEZIER_H
#define R64FX_GUI_BEZIER_H

#include <vector>
#include "geometry.h"

namespace r64fx{

Point<float> point_on_a_segment(Point<float> a, Point<float> b, float position);

void bezier_points(const std::vector<Point<float>> &in, std::vector<Point<float>> &out);

}//namespace r64fx

#endif//R64FX_GUI_BEZIER_H