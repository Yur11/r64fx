#ifndef R64FX_GUI_VERTEX_BUFFER_H
#define R64FX_GUI_VERTEX_BUFFER_H

#include "Shader.h"
#include "RenderingContext.h"
#include "Error.h"

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

namespace r64fx{

    
/** @brief A handy wrapper for vertex buffer objects. 
 */
template<typename VertexT> class Vertices : public RenderingContextAware{
    const int _nbytes;
    GLuint _vbo = 0;
    GLuint _vao[max_rendering_context_count];
    
public:
    Vertices(const int nbytes, VertexT* data = nullptr, GLenum usage = GL_STATIC_DRAW)
    : _nbytes(nbytes)
    {
        glGenBuffers(1, &_vbo);
        CHECK_FOR_GL_ERRORS;
        this->bindBuffer();
        glBufferData(GL_ARRAY_BUFFER, nbytes, data, usage);
        CHECK_FOR_GL_ERRORS;
        this->unbindBuffer();
        
        for(int i=0; i<max_rendering_context_count; i++)
            _vao[i] = 0;
    }
    
    ~Vertices()
    {
        glDeleteBuffers(1, &_vbo);
        CHECK_FOR_GL_ERRORS;
    }
    
//     inline unsigned int vertexCount() const { return _vertex_count; }
    inline int nbytes() const { return _nbytes; }
    
    inline GLuint id() const { return _vbo; }
    
    inline void bindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, _vbo); CHECK_FOR_GL_ERRORS;  }
    
    inline static void unbindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); CHECK_FOR_GL_ERRORS;  }
    
    inline void bindArray() { glBindVertexArray(_vao[RenderingContext::current()->id()]); CHECK_FOR_GL_ERRORS; }
    
    inline static void unbindArray() { glBindVertexArray(0); CHECK_FOR_GL_ERRORS;  }
    
    inline void setVertices(VertexT* vertices, int vertex_count, int index = 0)
    {
        glBufferSubData(GL_ARRAY_BUFFER, index, vertex_count * sizeof(VertexT), vertices);
        CHECK_FOR_GL_ERRORS;
    }
    
    virtual void setupForContext(RenderingContextId_t context_id)
    {
        std::cout << "Vertices::setupForContext " << context_id << "\n";
        
        if(_vao[context_id] !=0)
        {
#ifdef DEBUG
            std::cerr << "VertexBuffer: Extra setup for context: " << context_id << " !\n";
#endif//DEBUG
            return;
        }
        
        glGenVertexArrays(1, _vao + context_id);   CHECK_FOR_GL_ERRORS; 
        glBindVertexArray(_vao[context_id]);       CHECK_FOR_GL_ERRORS; 
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);       CHECK_FOR_GL_ERRORS; 
        VertexT::setupAttributes(nbytes());
        glBindVertexArray(0);                      CHECK_FOR_GL_ERRORS; 
    }
    
    virtual void cleanupForContext(RenderingContextId_t context_id)
    {
        std::cout << "Vertices::cleanupForContext " << context_id << "\n";
        
        if(_vao[context_id] == 0)
        {
#ifdef DEBUG
            std::cerr << "VertexBuffer: Extra cleanup for context: " << context_id << " !\n";
#endif//DEBUG
            return;
        }
        
        glDeleteVertexArrays(1, _vao + context_id);
    }
};

}//namespace r64fx

#endif//R64FX_GUI_VERTEX_BUFFER_H