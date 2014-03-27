#ifndef R64FX_GUI_PAINTER_H
#define R64FX_GUI_PAINTER_H

#include "Projection2D.h"
#include "Vertices.h"
#include "Shader.h"
#include "Color.h"


namespace r64fx{
    
/** @brief A general purpose painter used to draw a lot of gui elements. 
 
    All Painter instances share the same ShadingProgram that can draw textured primitives.
    
    Painter properies, such as color, texture or projection, are shared between instances, 
    thus are controled through static members.
    
    Painter uses 4 floats per vertex: 2D position and texture coordinates.
    The vbo is divided into two parts.
    Positions go into the first part (lower addresses), tex. coords. go into the second part (higher addresses).
    This allows us to update vertex positions in a single api call without touching tex. coords.
    
    The Vertices<> template provides automatic state management for multiple contexts.
 */
class Painter : public Vertices<Painter>{
    friend class Vertices<Painter>;
    
    static ShadingProgram sp;
    
    static GLint position_attr;
    static GLint tex_coord_attr;
    
    static GLint sxsytxty_uniform; //Scale and translation by x and y.
    static GLint color_uniform;
    static GLint sampler_uniform;
    static GLint texturing_mode_uniform;
    
    static GLuint plain_tex;
    
    static int current_texturing_mode;
    
    /** Called by the Vertices<> base class.
        For a specific vao.
     
        This function calls glEnableVertexAttribArray() and glVertexAttribPointer().
     */
    static void setupAttributes(int nbytes);
    
public:
    /** @brief Create a new Painter. 
     
        @param nvertices - sets the maximum number of vertices that this Painter can render.
     */
    Painter(int nvertices)
    : Vertices<Painter>(nvertices * sizeof(float) * 4)
    {}
    
    /** @brief Initialize the Painter operations. 
     
        This creates the shader and the default white texture.
        An OpenGL context must be available when calling this function.
        
        @returns true on success.
     */
    static bool init();
    
    /** @brief The max. number of vertices that this Painter can draw. */
    inline int size() const { return nbytes()>>4; }
    
    /** @brief Activates the ShadingProgram used by the Painter. 
     
        This function calls glUseProgram().
     */
    inline static void enable() { sp.use(); };
    
    /** @brief Calls glUseProgram(0); */
    inline static void disable() { gl::UseProgram(0); }
    
    static const int RGBA = 1;
    
    static const int RedAsAlpha = 2;
    
    inline static void setTexturingMode(unsigned int mode) { gl::Uniform1i(texturing_mode_uniform, mode); current_texturing_mode = mode; }
    
    inline static int currentTexturingMode() { return current_texturing_mode; }
    
    inline static void useCurrent2dProjection() { gl::Uniform4fv(sxsytxty_uniform, 1, current_2d_projection->vec); }
    
    inline static void setProjection(Projection2D projection) { gl::Uniform4fv(sxsytxty_uniform, 1, projection.vec); }
    
    inline static void setColor(Color color) { gl::Uniform4fv(color_uniform, 1, color.vec); }
    
    inline static void setColor(float r, float g, float b, float a) { setColor({r, g, b, a}); }
    
    static void setTexture(GLuint tex);
    
    /** @brief Equivalent to calling Painter::setTexture(Painter::plainTexture()). */
    inline static void useNoTexture() { setTexture( Painter::plainTexture() ); }
    
    /** @brief Use a plain white texture. */
    inline static GLuint plainTexture() { return plain_tex; }
    
    /** @brief Set the vertex data. 
     
        The data must be aligned as follows.
        
        \code
        float data[ 4 * N * sizeof(float) ] = {
            Vertex positions.
            N vertices 2 floats each. (x, y)
            
            Tex. coords.
            N vertices 2 floats each. (s, t)
        };
        \endcode
        
        Where N is the max vertex count that can be drawn by this Painter.
        Use the size() method to obtain this value.
     */
    void setData(float *ptr);
    
    /** @brief Update the first half of the vbo. 
        
        @param ptr - pointer to the input data.
        
        @param nfloats - the number of 32bit floating point values to update.
        
        @param offset - offset from the beginning of the buffer, specified as the number of 32bit floats.
        
     */
    void setPositions(float* ptr, int nfloats, int offset = 0);
    
    /** @brief Update the second half of the vbo. 
        
        @param ptr - pointer to the input data.
        
        @param nfloats - the number of 32bit floating point values to update.
        
        @param offset - offset from the beginning of the buffer, specified as the number of 32bit floats.
        
     */
    void setTexCoords(float* ptr, int nfloats, int offset = 0);
    
    /** @brief Render all the vertices of this Painter using the given connectivity. 
     
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

#endif//R64FX_GUI_PAINTER_H