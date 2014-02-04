#ifndef R64FX_GUI_TEXTURED_RECT_H
#define R64FX_GUI_TEXTURED_RECT_H

#include "Shader.h"
#include "RenderingContext.h"

namespace r64fx{
    
/** @brief A rendering method shared by several parts of the program. */
class TexturedRect : public RenderingContextAware{
    static TexturedRect* singleton_instance;
    static GLuint vao[max_rendering_context_count];
    static GLuint vbo;
    static ShadingProgram shading_program;
    static GLint vertex_coord_attribute;
    static GLint geometry_uniform;
    static GLint tex_coord_uniform;
    static GLuint sampler;
    static GLint sampler_uniform;
    
    TexturedRect() {}

    virtual void setupForContext(RenderingContextId_t context_id);
    
    virtual void cleanupForContext(RenderingContextId_t context_id);

    virtual ~TexturedRect() {};
    
public:
    static bool init();
    
    inline static TexturedRect* instance() { return singleton_instance; }
    
    static void render(RenderingContextId_t context_id, float x, float y, float w, float h, float tex_x, float tex_y, float tex_w, float tex_h, GLuint tex);
    
    static void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXTURED_RECT_H