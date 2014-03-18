#ifndef R64FX_MAIN_DENSEWAVEFORMPAINTER_H
#define R64FX_MAIN_DENSEWAVEFORMPAINTER_H

#include "gui/Projection2D.h"
#include "gui/Vertices.h"
#include "gui/Shader.h"
#include "gui/Color.h"


namespace r64fx{
    

class DenseWaveformPainter : public Vertices<DenseWaveformPainter>{
    friend class Vertices<DenseWaveformPainter>;
    
    static ShadingProgram sp;
    
    static GLint position_attr;
    static GLint ref_color_attr;
    static GLint tex_coord_attr;
    
    static GLint sxsytxty_uniform; //Scale and translation by x and y.
    static GLint sampler_uniform;
        
    /** Called by the Vertices<> base class.
        For a specific vao.
     
        This function calls glEnableVertexAttribArray() and glVertexAttribPointer().
     */
    static void setupAttributes(int nbytes);
    
#ifdef DEBUG
    static GLuint debug_tex;
#endif//DEBUG
    
public:
    /** @brief Create a new DenseWaveformPainter. 
     
        @param nvertices - sets the maximum number of vertices that this DenseWaveformPainter can render.
     */
    DenseWaveformPainter(int nvertices)
    : Vertices<DenseWaveformPainter>(nvertices * sizeof(float) * 4)
    {}
    
    /** @brief Initialize the DenseWaveformPainter operations. 
     
        This creates the shader and the default white texture.
        An OpenGL context must be available when calling this function.
        
        @returns true on success.
     */
    static bool init();
    
    /** @brief The max. number of vertices that this DenseWaveformPainter can draw. */
    inline int size() const { return nbytes()>>4; }
    
    /** @brief Activates the ShadingProgram used by the DenseWaveformPainter. 
     
        This function calls glUseProgram().
     */
    inline static void enable() { sp.use(); };
    
    /** @brief Calls glUseProgram(0); */
    inline static void disable() { glUseProgram(0); CHECK_FOR_GL_ERRORS;  }
    
    inline static void useCurrent2dProjection() { glUniform4fv(sxsytxty_uniform, 1, current_2d_projection->vec); CHECK_FOR_GL_ERRORS;  }
    
    inline static void setProjection(Projection2D projection) { glUniform4fv(sxsytxty_uniform, 1, projection.vec); CHECK_FOR_GL_ERRORS;  }
    
    inline static void setColor(Color color) { /*glUniform4fv(color_uniform, 1, color.vec); CHECK_FOR_GL_ERRORS;*/  }
    
    inline static void setColor(float r, float g, float b, float a) { setColor({r, g, b, a}); }
    
    static void setTexture(GLuint tex);
    
#ifdef DEBUG
    inline static GLuint debugTex() { return debug_tex; }
#endif//DEBUG
    
    /** @brief Set the vertex data. 
     
        The data must be aligned as follows.
        
        \code
        float data[ 4 * N * sizeof(float) ] = {
            Vertex positions.
            N vertices 2 floats each. (x, y)
            
            ...
        };
        \endcode
        
        Where N is the max vertex count that can be drawn by this DenseWaveformPainter.
        Use the size() method to obtain this value.
     */
    void setData(float *ptr);
    
    /** @brief Update the first half of the vbo. 
        
        @param ptr - pointer to the input data.
        
        @param nfloats - the number of 32bit floating point values to update.
        
        @param offset - offset from the beginning of the buffer, specified as the number of 32bit floats.
        
     */
    void setPositions(float* ptr, int nfloats, int offset = 0);
    
    /** @brief Render all the vertices of this DenseWaveformPainter using the given connectivity. 
     
        This function calls glDrawArrays().
     */
    inline void render(GLenum mode) { render(mode, size()); }
    
    /** @brief Render some of the vertices. 
     
        This function calls glDrawArrays().
        The offset is measured in vertices.
        
        One can simply call glDrawArrays() instead of this function.
     */
    void render(GLenum mode, int nvertices, int offset = 0);
};

}//namespace r64fx

#endif//R64FX_MAIN_DENSEWAVEFORMPAINTER_H