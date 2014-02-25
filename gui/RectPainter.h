#ifndef R64FX_GUI_RECT_PAINTER_H
#define R64FX_GUI_RECT_PAINTER_H

#include "Shader.h"
#include "RenderingContext.h"
#include "geometry.h"

namespace r64fx{
    
/** @brief A rendering method shared by several parts of the program. */
class RectPainter : public RenderingContextAware{
    static RectPainter* singleton_instance;
    static GLuint vao[max_rendering_context_count];
    static GLuint vbo;
    static ShadingProgram shading_program;
    static GLint vertex_coord_attribute;
    static GLint geometry_uniform;
    static GLint tex_coord_uniform;
    static GLint scale_and_shift_uniform;
    static GLint sampler_uniform;
    static GLint color_uniform;
    static GLuint plain_tex;
    
    RectPainter() {}
    
    RectPainter(const RectPainter&) {}

    virtual void setupForContext(RenderingContextId_t context_id);
    
    virtual void cleanupForContext(RenderingContextId_t context_id);

    virtual ~RectPainter() {};
    
public:
    static bool init();
    
    static inline GLuint plainTexture() { return plain_tex; }
    
    inline static RectPainter* instance() { return singleton_instance; }
    
    static void prepare();
    
    static void useCurrent2dProjection();
    
    static void setCoords(float* vec);
    
    inline static void setCoords(float x, float y, float w, float h) 
    {
        float vec[4] = { x, y, w, h };
        setCoords(vec);
    }
    
    inline static void setCoords(Point<float> p, Size<float> s)
    {
        setCoords(p.x, p.y, s.w, s.h);
    }
    
    static void setTexture(GLuint tex);
    
    static void setTexCoords(float* vec);
    
    inline static void setTexCoords(float tex_x, float tex_y, float tex_w, float tex_h)
    {
        float vec[4] = { tex_x, tex_y, tex_w, tex_h };
        setTexCoords(vec);
    }
    
    static void setColor(float* vec);
    
    inline static void setColor(float r, float g, float b, float a = 0.0)
    {
        float vec[4] = { r, g, b, a };
        setColor(vec);
    }
    
    static void render();
    
    static void renderOutline();
            
    static void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_GUI_RECT_PAINTER_H