#include "RectPainter.h"
#include "Projection2D.h"
#include "Error.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
using namespace std;
#endif//DEBUG


namespace r64fx{
   
RectPainter*       RectPainter::singleton_instance;
GLuint             RectPainter::vao[max_rendering_context_count];
GLuint             RectPainter::vbo;
ShadingProgram     RectPainter::shading_program;
GLint              RectPainter::vertex_coord_attribute;
GLint              RectPainter::geometry_uniform;
GLint              RectPainter::tex_coord_uniform;
GLint              RectPainter::scale_and_shift_uniform;
GLint              RectPainter::sampler_uniform;
GLint              RectPainter::color_uniform;
GLuint             RectPainter::plain_tex;
    
    
bool RectPainter::init()
{
    VertexShader vs(
        #include "RectPainter.vert.h"
    );
    
    if(!vs.isOk())
    {
        cerr << "Error in vertex shader!\n";
        cerr << vs.infoLog() << "\n";
        return false;
    }
    
    FragmentShader fs(
        #include "RectPainter.frag.h"
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
    CHECK_FOR_GL_ERRORS; 
#ifdef DEBUG
    assert(vertex_coord_attribute != -1);
#endif//DEBUG
    
    geometry_uniform           = glGetUniformLocation(shading_program.id(), "geometry");
    CHECK_FOR_GL_ERRORS; 
    tex_coord_uniform          = glGetUniformLocation(shading_program.id(), "tex_coord");
    CHECK_FOR_GL_ERRORS; 
    scale_and_shift_uniform    = glGetUniformLocation(shading_program.id(), "scale_and_shift");
    CHECK_FOR_GL_ERRORS; 
    sampler_uniform            = glGetUniformLocation(shading_program.id(), "sampler");
    CHECK_FOR_GL_ERRORS; 
    color_uniform              = glGetUniformLocation(shading_program.id(), "color");
    CHECK_FOR_GL_ERRORS; 
    
#ifdef DEBUG
    assert(geometry_uniform != -1);
    assert(tex_coord_uniform != -1);
    assert(scale_and_shift_uniform != -1);
    assert(sampler_uniform != -1);
    assert(color_uniform != -1);
#endif//DEBUG
    
    
    float vbo_data[8] = {
        0.0, 0.0,  1.0, 0.0,
        1.0, 1.0,  0.0, 1.0
    };

    glGenBuffers(1, &vbo);
    CHECK_FOR_GL_ERRORS; 
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CHECK_FOR_GL_ERRORS; 
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vbo_data, GL_STATIC_DRAW);
    CHECK_FOR_GL_ERRORS; 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_FOR_GL_ERRORS; 
    
    unsigned char plain_tex_data[16 * 16 * 4];
    for(int i=0; i<16*16*4; i++)
        plain_tex_data[i] = 255;
        
    glGenTextures(1, &plain_tex);
    CHECK_FOR_GL_ERRORS; 
    glBindTexture(GL_TEXTURE_2D, plain_tex);
    CHECK_FOR_GL_ERRORS; 
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    CHECK_FOR_GL_ERRORS; 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, plain_tex_data);
    CHECK_FOR_GL_ERRORS; 
    
    singleton_instance = new RectPainter;
    
    return true;
}


void RectPainter::setupForContext(RenderingContextId_t context_id)
{
    cout << "RectPainter::setupForContext: " << context_id << "\n";

    if(vao[context_id] !=0)
    {
#ifdef DEBUG
        cerr << "RectPainter: Extra setup for context: " << context_id << " !\n";
#endif//DEBUG
        return;
    }
    
    glGenVertexArrays(1, vao + context_id);
    CHECK_FOR_GL_ERRORS; 
    glBindVertexArray(vao[context_id]);
    CHECK_FOR_GL_ERRORS; 
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CHECK_FOR_GL_ERRORS; 
    glEnableVertexAttribArray(vertex_coord_attribute);
    CHECK_FOR_GL_ERRORS; 
    glVertexAttribPointer(vertex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    CHECK_FOR_GL_ERRORS; 
    glBindVertexArray(0);
    CHECK_FOR_GL_ERRORS; 
}

    
void RectPainter::cleanupForContext(RenderingContextId_t context_id)
{
    cout << "RectPainter::cleanupForContext: " << context_id << "\n";
    
    if(vao[context_id] == 0)
    {
#ifdef DEBUG
        cerr << "RectPainter: Extra cleanup for context: " << context_id << " !\n";
#endif//DEBUG
        return;
    }
    
    glDeleteVertexArrays(1, vao + context_id);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::prepare()
{
    shading_program.use();
    useCurrent2dProjection();
}


void RectPainter::useCurrent2dProjection()
{
    glUniform4fv(scale_and_shift_uniform, 1, current_2d_projection->vec);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::setCoords(float* vec)
{
    glUniform4fv(geometry_uniform, 1, vec);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::setTexture(GLuint tex)
{
    glActiveTexture(GL_TEXTURE0 + 0);
    CHECK_FOR_GL_ERRORS;
    glBindTexture(GL_TEXTURE_2D, tex);
    CHECK_FOR_GL_ERRORS; 
//     glBindSampler(0, sampler);
//     CHECK_FOR_GL_ERRORS; 
    glUniform1i(sampler_uniform, 0);
    CHECK_FOR_GL_ERRORS;
}


void RectPainter::setTexCoords(float* vec)
{
    glUniform4fv(tex_coord_uniform, 1, vec);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::setColor(float* vec)
{
    glUniform4fv(color_uniform, 1, vec);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::render()
{
    glBindVertexArray(vao[RenderingContext::current()->id()]);
    CHECK_FOR_GL_ERRORS; 
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    CHECK_FOR_GL_ERRORS; 
    glBindVertexArray(0);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::renderOutline()
{
    glBindVertexArray(vao[RenderingContext::current()->id()]);
    CHECK_FOR_GL_ERRORS; 
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    CHECK_FOR_GL_ERRORS; 
    glBindVertexArray(0);
    CHECK_FOR_GL_ERRORS; 
}


void RectPainter::cleanup()
{
    singleton_instance->discard();
}
    
}//namespace r64fx