#ifndef R64_FX_GUI_ROTATIONS_H
#define R64_FX_GUI_ROTATIONS_H

#include <vector>
#include "geometry.h"


namespace r64fx{
    
/** @brief Make an angle value fit into the 0 .. 360 deg. range. */
float normalize360(float angle);


float normalize2pi(float angle);


/** @brief Convert degrees to radians. */
float deg2rad(float angle);

    
/** @brief Calculate vertex brighteness for drawing a fake rotational solid. 
 
    Based on the angle of the vertex relative to the light angle.
    Vertex angle that matches the light_angle produces the value of 1.0.
    The angle opposite to the light angle gives 0.0.
 */
float vertex_brightness(float angle, float light_angle);

struct LitVertex{
    Point<float> p;
    float brightness;
    
    LitVertex(Point<float> p = {0.0, 0.0}, float brightness = 0.0)
    : p(p)
    , brightness(brightness)
    {}
};


/** @brief Calculate vertex positions and brightness values that lie on a circle and is lit up at a specific angle.
 
    This function uses two different radii, that are interchanged when calculated each consecutive vertex.
    If both of the radii are the same the vertices will lie on circle, otherwise they will form for kind of a star.
 
    Prefill the output vector with the number of vertices you wish to recieve.
 */
void circle_vertices(std::vector<LitVertex> &vertices, float start_angle, float light_angle, float radius1, float radius2);
    
}//namespace r64fx

#endif//R64_FX_GUI_ROTATIONS_H