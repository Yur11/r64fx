#include "bezier.hpp"

using namespace std;

namespace r64fx{
    
Point<float> point_on_a_segment(Point<float> a, Point<float> b, float position)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return Point<float>(a.x + dx * position, a.y + dy * position);
}


Point<float> bezier_point(const std::vector<Point<float>> &in, float position)
{
#ifdef DEBUG
    assert(in.size() >= 2);
#endif//DEBUG
    
    if(in.size() == 2)
        return point_on_a_segment(in[0], in[1], position);
        
    vector<Point<float>> vec;
    for(int i=0; i<(int)in.size()-1; i++)
        vec.push_back(point_on_a_segment(in[i], in[i+1], position));
    
    return bezier_point(vec, position);
}

    
}//namespace r64fx