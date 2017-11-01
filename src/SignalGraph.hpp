#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "jit.hpp"

#define R64FX_NODE_SOURCE(name) private: SignalSource m_##name; public: inline NodeSource name() { return {this, &m_##name}; } private:
#define R64FX_NODE_SINK(name)   private: SignalSink   m_##name; public: inline NodeSink   name() { return {this, &m_##name}; } private:

namespace r64fx{

class SignalNode;
class SignalGraphCompiler;


class DataBufferPointer{
    friend class SignalGraphCompiler;
    friend class SignalDataStorage;
    unsigned int m_offset = 0;
    DataBufferPointer(unsigned int offset) : m_offset(offset) {}
    inline unsigned int offset() const { return m_offset; }

public:
    DataBufferPointer() {}

    inline operator bool() const { return m_offset; }
};


class SignalDataStorage{
    friend class SignalGraphCompiler;
    union{
        unsigned long q = 0;
        unsigned int  d[2];
    }u;

public:
    SignalDataStorage() {}

    inline operator bool() const { return u.q; }

    inline bool isMemory()    const { return (u.d[1] & 0x30000000) == 0; }
    inline bool isRegister()  const { return (u.d[1] & 0x30000000) >= 0x10000000; }
    inline bool isGPR()       const { return (u.d[1] & 0x30000000) == 0x10000000; }
    inline bool isVector()    const { return (u.d[1] & 0x30000000) >= 0x20000000; }
    inline bool isXmm()       const { return (u.d[1] & 0x30000000) == 0x20000000; }
    inline bool isYmm()       const { return (u.d[1] & 0x30000000) == 0x30000000; }

    inline bool isSingle()    const { return !isDouble(); }
    inline bool isDouble()    const { return u.d[1] & 0x80000000; }
    inline void isDouble(bool yes)  { u.d[1] &= 0x7FFFFFFF; u.d[1] |= (yes ? 0x80000000 : 0); }

    inline unsigned int size() const { return u.d[1] & 0x0FFFFFFF; }
    inline void setSize(unsigned int size)
        { R64FX_DEBUG_ASSERT(size <= 0x0FFFFFFF); u.d[1] &= 0xF0000000; u.d[1] |= size; }

private:
    inline void clear() { u.q = 0; }

    inline void setMemory() { u.d[1] &= 0x30000000; }
    inline void setGPR()    { u.d[1] &= 0x30000000; u.d[1] |= 0x10000000; }
    inline void setXmm()    { u.d[1] &= 0x30000000; u.d[1] |= 0x20000000; }
    inline void setYmm()    { u.d[1] &= 0x30000000; u.d[1] |= 0x30000000; }
};


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource : public SignalDataStorage{
    friend class SignalGraphCompiler;
    SignalNode* m_parent_node = nullptr;
    unsigned int m_connected_sink_count = 0;
    unsigned int m_processed_sink_count = 0;

public:
    SignalSource(SignalNode* parent_node)
    : m_parent_node(parent_node) {}

    ~SignalSource() {}

    inline SignalNode* parentNode() const { return m_parent_node; }

    inline int connectedSinkCount() const { return m_connected_sink_count; }

    inline int processedSinkCount() const { return m_processed_sink_count; }
};


/* Each SignalSink can be connected to only one SignalSource. */
class SignalSink{
    friend class SignalGraphCompiler;

    SignalSource*  m_connected_source  = nullptr;

public:
    SignalSink() {}

    ~SignalSink() {}

    inline SignalSource* connectedSource() const { return m_connected_source; }
};


class SignalNode{
    friend class SignalGraphCompiler;

    unsigned long m_iteration_count = 0;
    unsigned long m_connection_count = 0;

public:
    SignalNode() {}

    virtual ~SignalNode() {}

private:
    virtual void build(SignalGraphCompiler &c) = 0;

    virtual void cleanup(SignalGraphCompiler &c) = 0;
};


template<typename SignalPortT> class NodePort{
    SignalNode*   m_node  = nullptr;
    SignalPortT*  m_port  = nullptr;

public:
    NodePort(SignalNode* node, SignalPortT* port)
    : m_node(node), m_port(port) {}

    inline SignalNode* node() const { return m_node; }

    inline SignalPortT* port() const { return m_port; }
};

typedef NodePort<SignalSource> NodeSource;
typedef NodePort<SignalSink> NodeSink;


class SignalGraphCompiler : public Assembler{
    unsigned long  m_iteration_count = 0;
    unsigned long  m_flags        = 0;
    unsigned long  m_frame_count  = 0;
    unsigned long  m_gprs[16];
    unsigned long  m_xmms[16];

public:
    SignalGraphCompiler();

    void link(const NodeSource &node_source, const NodeSink &node_sink);

    void unlink(const NodeSink node_sink);

    void build(SignalNode* terminal_nodes, unsigned int nnodes);

    void ensureBuilt(SignalSource* source);

private:
    void buildNode(SignalNode* node);

public:
    inline long run() { return ((long (*)())codeBegin())(); }

    inline unsigned long frameCount() const { return m_frame_count; }

    inline void setFrameCount(unsigned long frame_count) { m_frame_count = frame_count; }

    inline GPR64 mainLoopCounter() const { return rcx; }


    DataBufferPointer allocMemory(unsigned int nbytes, unsigned int align);

    void freeMemory(DataBufferPointer ptr);

    template<typename MemT> inline MemT ptrMem(DataBufferPointer ptr) const 
        { R64FX_DEBUG_ASSERT(ptr); return MemT(codeBegin() - ptr.m_offset); };


    unsigned int allocGPR(GPR64* gprs, unsigned int ngprs);

    void freeGPR(GPR64* gprs, unsigned int ngprs);


    unsigned int allocXmm(Xmm* xmms, unsigned int nxmms);

    void freeXmm(Xmm* xmms, unsigned int nxmms);


    void setStorage(SignalDataStorage &storage, DataBufferPointer ptr);

    DataBufferPointer getPtr(SignalDataStorage storage) const;

    void setStorage(SignalDataStorage &storage, GPR* gprs, unsigned int ngprs);

    void getStorage(SignalDataStorage storage, GPR* gprs, unsigned int* ngprs);

    void setStorage(SignalDataStorage &storage, Xmm* xmms, unsigned int nxmms);

    void getStorage(SignalDataStorage storage, Xmm* xmms, unsigned int* nxmms);

    void freeStorage(SignalDataStorage &storage);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
