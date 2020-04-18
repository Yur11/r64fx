#ifdef R64FX_USE_GL

#ifndef R64FX_GUI_SHADER_H
#define R64FX_GUI_SHADER_H

#include "gl.hpp"

namespace r64fx{

class Shader{
    GLuint m_shader = 0;

protected:
    Shader(const char** text, GLsizei count, GLenum shader_type);

    Shader(const char* text, GLenum shader_type) : Shader(&text, 1, shader_type) {}

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
    bool load(VertexShader vs, FragmentShader fs);

    void free();

    bool isGood();

    const char* infoLog();

    void use();

    inline GLuint id() const { return m_program; }

    inline VertexShader vertexShader() const { return m_vs; }

    inline FragmentShader fragmentShader() const { return m_fs; }

    void getAttribLocation(GLint &out, const char* name);

    void getUniformLocation(GLint &out, const char* name);

    void bindAttribute(GLuint attr, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);
};

}//namespace r64fx

#endif//R64FX_GUI_SHADER_H

#endif//R64FX_USE_GL
