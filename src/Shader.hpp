#ifndef R64FX_GUI_SHADER_H
#define R64FX_GUI_SHADER_H

#include "gl.hpp"

namespace r64fx{
    
class Shader{
    GLuint m_shader = 0;
    
protected:
    Shader(const char* text, GLenum shader_type);
    
    Shader() {}
    
public:
    bool isOk();
    
    const char* infoLog();
    
    inline GLuint id() const { return m_shader; }
    
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
    VertexShader m_vs;
    FragmentShader m_fs;
    GLuint m_program = 0;
    
public:
    ShadingProgram(VertexShader vs, FragmentShader fs);
    
    virtual ~ShadingProgram();

    bool isOk();
    
    const char* infoLog();
    
    void use();
    
    inline GLuint id() const { return m_program; }
    
    inline VertexShader vertexShader() const { return m_vs; }
    
    inline FragmentShader fragmentShader() const { return m_fs; }
    
    void getAttribLocation(GLint &out, const char* name);

    void getUniformLocation(GLint &out, const char* name);
};


class VertexArray{
    int m_vertex_count = 0;
    GLuint m_vao;

public:
    VertexArray(int vertex_count);

    virtual ~VertexArray();

    inline int vertexCount() const { return m_vertex_count; }

    inline GLuint vao() const { return m_vao; }

    void bind();
};

}//namespace r64fx

#endif//R64FX_GUI_SHADER_H