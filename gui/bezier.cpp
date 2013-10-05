#include "bezier.h"

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
Point<float> point_on_a_segment(Point<float> a, Point<float> b, float position)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return Point<float>(a.x + dx * position, a.y + dy * position);
}



void bezier_points(const std::vector<Point<float>> &in, std::vector<Point<float>> &out)
{
#ifdef DEBUG
    assert(in.size() >= 2);
    assert(out.size() >= 1);
#endif//DEBUG
    
    float step = 1.0 / out.size();
    float position = step;

    out.back() = in.back();
    out.front() = in.front();

    vector<Point<float>> intermadiate_points = in;
    for(int i=1; i<(int)out.size()-1; i++)
    {
        int n = intermadiate_points.size() - 1;
        while(n > 1)
        {
            for(int j=0; j<n; j++)
            {
                intermadiate_points[j] = point_on_a_segment(intermadiate_points[j], intermadiate_points[j+1], position);
            }
            n--;
        }
        out[i] = point_on_a_segment(intermadiate_points[0], intermadiate_points[1], position);
        position += step;
    }
}
    
}//namespace r64fx