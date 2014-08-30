#ifndef R64FX_GUI_SHADER_H
#define R64FX_GUI_SHADER_H

#include "gl.hpp"

namespace r64fx{
    
class Shader{
    GLuint _shader = 0;
    
protected:
    Shader(const char* text, GLenum shader_type);
    
    Shader() {}
    
public:
    bool isOk();
    
    const char* infoLog();
    
    inline GLuint id() const { return _shader; }
    
    void free();
};


class VertexShader : public Shader{
public:
    VertexShader(const char* text) : Shader(text, GL_VERTEX_SHADER) {}
    
    VertexShader() {}
};


class FragmentShader : public Shader{
public:
    FragmentShader(const char* text) : Shader(text, GL_FRAGMENT_SHADER) {}  
    
    FragmentShader() {}
};


class ShadingProgram{
    VertexShader _vs;
    FragmentShader _fs;
    GLuint _program = 0;
    
public:
    ShadingProgram(VertexShader vs, FragmentShader fs);
    
    ShadingProgram() {}
    
    bool isOk();
    
    const char* infoLog();
    
    void use();
    
    inline GLuint id() const { return _program; }
    
    inline VertexShader vertexShader() const { return _vs; }    
    
    inline FragmentShader fragmentShader() const { return _fs; }
    
    void free();
    
    /** @brief Create a shading program using the vertex shader and fragment shader texts. */     
    static ShadingProgram create(const char* vs_text, const char* fs_text);
};

    
}//namespace r64fx

#endif//R64FX_GUI_SHADER_H
