#include "Graph.h"
#ifdef DEBUG
#include <assert.h>
#endif//DEBUG


using namespace r64fx;

namespace r64fx{
    
    
Graph::Graph()
: as(cb)
{
}
    
    
Graph::~Graph()
{
    
    while(!_reading_connections.empty())
    {
        delete _reading_connections.back();
        _reading_connections.pop_back();
    }
    
    while(!_writing_connections.empty())
    {
        delete _writing_connections.back();
        _writing_connections.pop_back();
    }
    
    cb.cleanup();
}
    
    
BufferReadingConnection* Graph::readFromBuffer(float* buffer, float* addr)
{
    auto brc = new BufferReadingConnection;
    brc->_buffer = buffer;
    brc->_addr = addr;
    _reading_connections.push_back(brc);
    return brc;
}
    
    
BufferWritingConnection* Graph::writeToBuffer(float* buffer, float* addr)
{
    auto bwc = new BufferWritingConnection;
    bwc->_buffer = buffer;
    bwc->_addr = addr;
    _writing_connections.push_back(bwc);
    return bwc;
}


void Graph::breakConnection(BufferReadingConnection* brc)
{
    auto it = _reading_connections.begin();
    while(it != _reading_connections.end() || *it != brc) 
        it++;
#ifdef DEBUG
    assert(it != _reading_connections.end());
#endif//DEBUG
    _reading_connections.erase(it);
    delete brc;
}


void Graph::breakConnection(BufferWritingConnection* bwc)
{
    auto it = _writing_connections.begin();
    while(it != _writing_connections.end() || *it != bwc) 
        it++;
#ifdef DEBUG
    assert(it != _writing_connections.end());
#endif//DEBUG
    _writing_connections.erase(it);
    delete bwc;
}
    
    
void Graph::render(int nsamples)
{
    cb.rewind();
    
#ifdef DEBUG
    as.dump << "\n\n---Begin---\n";
#endif//DEBUG
    as.mov(r15, 0); //Buffer position in bytes.
    as.mov(r14, nsamples);
    
    /* Main processing loop */
    auto loop = Mem8(as.ip());
    
#ifdef DEBUG
    as.dump << "\n\n---Reading from buffers---\n";
#endif//DEBUG
    for(auto rc : _reading_connections)
        rc->render(as);
    
#ifdef DEBUG
    as.dump << "\n\n---Processors---\n";
#endif//DEBUG/
    for(auto p : processors)
    {
        p->render(as);
        p->renderRouting(as);
    }
    
#ifdef DEBUG
    as.dump << "\n\n---Writing to buffers---\n";
#endif//DEBUG
    for(auto wc : _writing_connections)
        wc->render(as);
    
#ifdef DEBUG
    as.dump << "\n\n---End of loop---\n";
#endif//DEBUG
    as.add(r15, 4);
    as.sub(r14, Imm32(1));
    as.jnz(loop);
#ifdef DEBUG
    as.dump << "\n";
#endif//DEBUG
    
    as.ret();
#ifdef DEBUG
    as.dump << "\n\n---End---\n";
#endif//DEBUG
    
    _fun = (Fun) as.getFun();
}
    
    
void Graph::run()
{
#ifdef DEBUG
    assert(_fun != nullptr);
#endif//DEBUG
    
    _fun();
}
    
    
void BufferReadingConnection::render(Assembler &as)
{
    as.mov(rbx, Imm64(_buffer));
    as.add(rbx, r15);
    as.mov(rax, Base(rbx));
    as.mov(Mem32(_addr), eax);
}


void BufferWritingConnection::render(Assembler &as)
{
    as.mov(rbx, Imm64(_buffer));
    as.add(rbx, r15);
    as.mov(eax, Mem32(_addr));
    as.mov(Base(rbx), rax);
}
    
}//namespace r64fx