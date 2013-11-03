#include "rotations.h"
#include <cmath>

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif//DEBUG

namespace r64fx{
    
float normalize360(float angle)
{
    if(angle < 0) while(angle < 0)
        angle+=360;
    else if(angle > 360) while(angle > 360)
        angle-=360;
    return angle;
}


float normalize2pi(float angle)
{
    if(angle < 0) while(angle < 0)
        angle+=2*M_PI;
    else if(angle > 2*M_PI) while(angle > 2*M_PI)
        angle-=2*M_PI;
    return angle;
}


float deg2rad(float angle)
{
    return angle * M_PI / 180;
}


float vertex_brightness(float angle, float light_angle)
{
    float angle_diff = fabs(angle - normalize2pi(light_angle + M_PI));
    if(angle_diff > M_PI)
        angle_diff = 2.0 * M_PI - angle_diff;
    
    return pow(angle_diff / M_PI, 2);
}


const float m_pi_rcp = 1.0 / M_PI;

void circle_vertices(std::vector<LitVertex> &vertices, float start_angle, float light_angle, float radius1, float radius2)
{
    float step = 2*M_PI / vertices.size();
    float angle = start_angle;
    float radius = radius1;
    
    for(int i=0; i<(int)vertices.size(); i++)
    {
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        float brightness = vertex_brightness(normalize2pi(angle), light_angle);
        
        vertices[i] = {{x, y}, brightness};
        
        radius = (radius == radius2) ? radius1 : radius2; 
        angle += step;
    }
}
    
}//namespace r64fx