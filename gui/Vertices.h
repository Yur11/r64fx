#ifndef R64FX_GUI_VERTEX_BUFFER_H
#define R64FX_GUI_VERTEX_BUFFER_H

#include "Shader.h"
#include "RenderingContext.h"

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
        gl::GenBuffers(1, &_vbo);
        this->bindBuffer();
        gl::BufferData(GL_ARRAY_BUFFER, nbytes, data, usage);
        this->unbindBuffer();
        
        for(int i=0; i<max_rendering_context_count; i++)
            _vao[i] = 0;
    }
    
    ~Vertices()
    {
        gl::DeleteBuffers(1, &_vbo);
    }
    
    inline int nbytes() const { return _nbytes; }
    
    inline GLuint id() const { return _vbo; }
    
    inline void bindBuffer() { gl::BindBuffer(GL_ARRAY_BUFFER, _vbo);  }
    
    inline static void unbindBuffer() { gl::BindBuffer(GL_ARRAY_BUFFER, 0);  }
    
    inline void bindArray() { gl::BindVertexArray(_vao[RenderingContext::current()->id()]); }
    
    inline static void unbindArray() { gl::BindVertexArray(0); }
    
    inline void setVertices(VertexT* vertices, int vertex_count, int index = 0)
    {
        gl::BufferSubData(GL_ARRAY_BUFFER, index, vertex_count * sizeof(VertexT), vertices);
    }
    
    virtual void setupForContext(RenderingContextId_t context_id)
    {
//         std::cout << "Vertices::setupForContext " << context_id << "\n";
        
        if(_vao[context_id] !=0)
        {
#ifdef DEBUG
            std::cerr << "VertexBuffer: Extra setup for context: " << context_id << " !\n";
#endif//DEBUG
            return;
        }
        
        gl::GenVertexArrays(1, _vao + context_id); 
        gl::BindVertexArray(_vao[context_id]);
        gl::BindBuffer(GL_ARRAY_BUFFER, _vbo);
        VertexT::setupAttributes(nbytes());
        gl::BindVertexArray(0);
    }
    
    virtual void cleanupForContext(RenderingContextId_t context_id)
    {
//         std::cout << "Vertices::cleanupForContext " << context_id << "\n";
        
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