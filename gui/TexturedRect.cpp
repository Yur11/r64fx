#include "TexturedRect.h"
#include <iostream>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
   
GLuint             TexturedRect::rect_vbo;
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
    
    float vbo_data[8] = {
        0.0, 0.0,  1.0, 0.0,
        0.0, 1.0,  1.0, 1.0
    };

    glGenBuffers(1, &rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vbo_data, GL_STATIC_DRAW);
    
    vertex_coord_attribute = glGetAttribLocation(shading_program.id(), "vertex_coord");
#ifdef DEBUG
    assert(vertex_coord_attribute != -1);
#endif//DEBUG
    
    glVertexAttribPointer(vertex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_coord_attribute);
    
    geometry_uniform   = glGetUniformLocation(shading_program.id(), "geometry");
    tex_coord_uniform  = glGetUniformLocation(shading_program.id(), "tex_coord");
    sampler_uniform    = glGetUniformLocation(shading_program.id(), "sampler");
    
#ifdef DEBUG
    assert(geometry_uniform != -1);
    assert(tex_coord_uniform != -1);
    assert(sampler_uniform != -1);
#endif//DEBUG
    
    glGenSamplers(1, &sampler);
    
    return true;
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
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
    
}//namespace r64fx