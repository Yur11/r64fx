#ifndef R64FX_DSP_GRAPH_H
#define R64FX_DSP_GRAPH_H

#include "Processor.hpp"

namespace r64fx{
    
class BufferReadingConnection;
class BufferWritingConnection;
    
/** @brief DSP graph. */
class Graph{
    typedef void (*Fun)(void);
    Fun _fun = nullptr;
    
    /** @brief Jit assembler. */
    CodeBuffer cb;
    Assembler as;
    
    std::vector<BufferReadingConnection*> _reading_connections;
    std::vector<BufferWritingConnection*> _writing_connections;
    
public:
    Graph();
    
    ~Graph();
    
    BufferReadingConnection* readFromBuffer(float* buffer, float* addr);
    
    BufferWritingConnection* writeToBuffer(float* buffer, float* addr);
    
    void breakConnection(BufferReadingConnection* brc);
    
    void breakConnection(BufferWritingConnection* bwc);
    
    /** @brief A list of all processors. */
    std::vector<Processor*> processors;
    
    /** @brief Render a new processing function. */
    void render(int nsamples);
    
    /** @brief Run the current processing function. */
    void run();
    
#ifdef DEBUG
    inline std::string debug() { return as.dump.str(); }
#endif//DEBUG
};


class BufferReadingConnection{
    friend class Graph;
    float* _buffer;
    float* _addr;
    
    BufferReadingConnection() {}
public:
    
    void render(Assembler &as);
};


class BufferWritingConnection{
    friend class Graph;
    float* _buffer;
    float* _addr;

    BufferWritingConnection() {}
public:
    
    void render(Assembler &as);
};
 
}//namespace r64fx

#endif//R64FX_DSP_GRAPH_H