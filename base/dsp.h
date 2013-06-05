#ifndef R64FX_DSP_H
#define R64FX_DSP_H


namespace r64fx{
namespace dsp{

typedef void* (*Fun)(void*);
    
class Jit;


/** @brief A single parameter of a dsp node. */
class Parameter{
    /** Points to a signle scalar in the memory range of a VectorProcessor. */
    float* ptr;//Let's only use floats for now. Maybe this will change in the future.
};


/** @brief Base class for dsp nodes. 
 
    Subclasses are simply collections of parameters.
 */
class Node{
    virtual VectorProcessor* hostProcessor() = 0; //Needed mostly for dynamic_cast to work.
};

    
/** @brief A collection of nodes of the same kind, that get processed in parallel using simd capabilities of the cpu.  */
class VectorProcessor{
    /** @brief Render this node to machine code using the given jit compiler. */
    virtual void render(Jit*) = 0;
    
public:
    /** Allocate some nodes at this VectorProcessor. */
    Node* allocateNodes(int count);//?
    
    /** Drop some nodes at this VectorProcessor. */
    void dropNodes(Node*, int count);//?
    
};


/** @brief An edge between two nodes. */
class Connection{
    friend class Graph;
    
    Parameter* _a;
    Parameter* _b;
    
    Connection(Parameter* a, Parameter* b) : _a(a), _b(b) {}
public:
    
    inline Parameter* a() const { return _a; }
    inline Parameter* b() const { return _b; }
};


/** @brief A collection of nodes and edges(connections). */
class Graph{
public:
    /** @brief Create a new connection between two nodes. */
    Connection* makeConnection(Parameter* a, Parameter* b);
    
    /** @brief Drop an existing connection between two nodes. */
    void dropConnection(Connection*);
    
    /** @brief Render the graph into a function. */
    Fun render();
};

    
}//namespace dsp
}//namespace r64fx

#endif//R64FX_DSP_H
