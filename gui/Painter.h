#ifndef R64FX_GUI_PAINTER_H
#define R64FX_GUI_PAINTER_H

#include "Projection2D.h"
#include "Vertices.h"
#include "Shader.h"
#include "Color.h"


namespace r64fx{
    
/** @brief A general purpose painter used to draw a lot of gui elements. */
class Painter{
    static ShadingProgram sp;
    static GLint xyst_attr;
    static GLint sxsytxty_uniform;
    static GLint color_uniform;
    static GLint sampler_uniform;
    static GLuint sampler;
    static GLuint plain_tex;
    
public:
    static bool init();
    
    struct Vertex{
        float vec[4];
        
    public:
        Vertex(float x = 0.0, float y = 0.0, float s = 0.0, float t = 0.0);
        
        static void setupAttributes();
    };
    
    inline static void enable() { sp.use(); };
    
    inline static void disable() { glUseProgram(0); CHECK_FOR_GL_ERRORS;  }
    
    inline static void useCurrent2dProjection() { glUniform4fv(sxsytxty_uniform, 1, current_2d_projection->vec); CHECK_FOR_GL_ERRORS;  }
    
    inline static void setProjection(Projection2D projection) { glUniform4fv(sxsytxty_uniform, 1, projection.vec); CHECK_FOR_GL_ERRORS;  }
    
    inline static void setColor(Color color) { glUniform4fv(color_uniform, 1, color.vec); CHECK_FOR_GL_ERRORS;  }
    
    inline static void setTextureUnit(GLuint tex_unit) { glBindSampler(tex_unit, sampler); CHECK_FOR_GL_ERRORS;  }
    
    inline static GLuint plainTexture() { return plain_tex; }
    
    inline static void paint(GLenum mode, int nvertices, int offset = 0) { glDrawArrays(mode, offset, nvertices); CHECK_FOR_GL_ERRORS;  }
    
    template<typename T> void paintVertices(GLenum mode, T vertices, int offset = 0) { paint(mode, vertices.size(), offset); }
};
    
}//namespace r64fx

#endif//R64FX_GUI_PAINTER_H