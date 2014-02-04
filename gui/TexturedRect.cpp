#include "TexturedRect.h"
#include <iostream>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
   
TexturedRect*      TexturedRect::singleton_instance;
GLuint             TexturedRect::vao[max_rendering_context_count];
GLuint             TexturedRect::vbo;
ShadingProgram     TexturedRect::shading_program;
GLint              TexturedRect::vertex_coord_attribute;
GLint              TexturedRect::geometry_uniform;
GLint              TexturedRect::tex_coord_uniform;
GLuint             TexturedRect::sampler;
GLint              TexturedRect::sampler_uniform;
    
    
bool TexturedRect::init()
{
    VertexShader vs(
        #include "TexturedRect.vert.h"
    );
    
    if(!vs.isOk())
    {
        cerr << "Error in vertex shader!\n";
        cerr << vs.infoLog() << "\n";
        return false;
    }
    
    FragmentShader fs(
        #include "TexturedRect.frag.h"
    );
    
    if(!fs.isOk())
    {
        cerr << "Error in fragment shader!\n";
        cerr << fs.infoLog() << "\n";
        return false;
    }
    
    shading_program = ShadingProgram(vs, fs);
    if(!shading_program.isOk())
    {
        cerr << "Error with shading program!\n";
        cerr << shading_program.infoLog();
        return false;
    }
    
    vertex_coord_attribute = glGetAttribLocation(shading_program.id(), "vertex_coord");
#ifdef DEBUG
    assert(vertex_coord_attribute != -1);
#endif//DEBUG
    
    geometry_uniform   = glGetUniformLocation(shading_program.id(), "geometry");
    tex_coord_uniform  = glGetUniformLocation(shading_program.id(), "tex_coord");
    sampler_uniform    = glGetUniformLocation(shading_program.id(), "sampler");
    
#ifdef DEBUG
    assert(geometry_uniform != -1);
    assert(tex_coord_uniform != -1);
    assert(sampler_uniform != -1);
#endif//DEBUG
    
    glGenSamplers(1, &sampler);
    
    float vbo_data[8] = {
        0.0, 0.0,  1.0, 0.0,
        0.0, 1.0,  1.0, 1.0
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vbo_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    singleton_instance = new TexturedRect;
    
    return true;
}


void TexturedRect::setupForContext(RenderingContextId_t context_id)
{
    cout << "TexturedRect::setupForContext: " << context_id << "\n";

    if(vao[context_id] !=0)
    {
#ifdef DEBUG
        cerr << "TexturedRect: Extra setup for context: " << context_id << " !\n";
#endif//DEBUG
        return;
    }
    
    glGenVertexArrays(1, vao + context_id);
    glBindVertexArray(vao[context_id]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(vertex_coord_attribute);
    glVertexAttribPointer(vertex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
}

    
void TexturedRect::cleanupForContext(RenderingContextId_t context_id)
{
    if(vao[context_id] == 0)
    {
#ifdef DEBUG
        cerr << "TexturedRect: Extra cleanup for context: " << context_id << " !\n";
#endif//DEBUG
        return;
    }
    
    glDeleteVertexArrays(1, vao + context_id);
}


void TexturedRect::render(RenderingContextId_t context_id, float x, float y, float w, float h, float tex_x, float tex_y, float tex_w, float tex_h, GLuint tex)
{
    shading_program.use();
    float geom[4] = { x, y, w, h };
    glUniform4fv(geometry_uniform, 1, geom);
    float tex_coord[4] = { tex_x, tex_y, tex_w, tex_h };
    glUniform4fv(tex_coord_uniform, 1, tex_coord);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindSampler(tex, sampler);
    glUniform1f(sampler_uniform, sampler);
    
    glBindVertexArray(vao[context_id]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void TexturedRect::cleanup()
{
    singleton_instance->discard();
}
    
}//namespace r64fx