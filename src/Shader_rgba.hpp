#ifndef R64FX_SHADER_RGBA_HPP
#define R64FX_SHADER_RGBA_HPP

#include "Shader.hpp"

namespace r64fx{

class VertexArray_rgba;

/** @brief ShadingProgram with single color per vertex. */
class Shader_rgba : public ShadingProgram{
    friend class VertexArray_rgba;
    GLint attr_position;
    GLint attr_color;
    GLint unif_sxsytxty;

public:
    Shader_rgba();

    virtual ~Shader_rgba();

    void setScaleAndShift(float sx, float sy, float tx, float ty);
};


class VertexArray_rgba : public VertexArray{
    GLuint m_position_vbo;
    GLuint m_color_vbo;

public:

    /** @param size Measured in vertices. */
    VertexArray_rgba(Shader_rgba* shader, int size);

    virtual ~VertexArray_rgba();

    /** @brief Load vertex positions.

        @param points Pointer to vertex data. (x,y) interleaved.

        @param offset Measured in vertices.

        @param count The number of vertices to load.
     */
    void loadPositions(float* points, int offset, int count);

    /** @brief Load vertex colors.

        @param colors Pointer to vertex data. (r,g,b,a) interleaved.

        @param offset Measured in vertices.

        @param count The number of vertices to load.
     */
    void loadColors(unsigned char* colors, int offset, int count);
};


}//namespace r64fx

#endif//R64FX_SHADER_RGBA_HPP