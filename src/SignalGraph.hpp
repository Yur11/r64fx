#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "FlagUtils.hpp"
#include "jit.hpp"

#define R64FX_NODE_SOURCE(name) private: SignalSource m_##name; public: inline NodeSource name() { return {this, &m_##name}; } private:
#define R64FX_NODE_SINK(name)   private: SignalSink   m_##name; public: inline NodeSink   name() { return {this, &m_##name}; } private:

namespace r64fx{

class SignalNode;
class SignalGraphCompiler;

class SignalDataType          : R64FX_FLAG_TYPE(0xF000000000000000UL);

/* Basic constituents of SignalDataType. */
class SignalDataStorageType   : R64FX_FLAG_TYPE(0x3000000000000000UL); //Memory, GPR or Xmm
class SignalDataScalarSize    : R64FX_FLAG_TYPE(0x4000000000000000UL); //Single or Double
class SignalDataScalarType    : R64FX_FLAG_TYPE(0x8000000000000000UL); //Float or Int

R64FX_COMBINE_3_FLAG_TYPES(SignalDataType,  SignalDataStorageType, SignalDataScalarSize, SignalDataScalarType);


class DataBufferPointer{
    friend class SignalGraphCompiler;
    friend class SignalDataStorage;
    unsigned int m_offset = 0;
    DataBufferPointer(unsigned int offset) : m_offset(offset) {}
    inline unsigned int offset() const { return m_offset; }

public:
    inline operator bool() const { return m_offset; }
};


class SignalDataStorage{
    friend class SignalGraphCompiler;

    unsigned long m_bits = 0;

public:
    SignalDataStorage() {}

    inline operator bool() const { return m_bits; }

    inline static SignalDataStorageType Memory()  { return SignalDataStorageType (0x0000000000000000UL); }
    inline static SignalDataStorageType GPR()     { return SignalDataStorageType (0x1000000000000000UL); }
    inline static SignalDataStorageType Xmm()     { return SignalDataStorageType (0x2000000000000000UL); }

    inline static SignalDataScalarSize  Single()  { return SignalDataScalarSize  (0x0000000000000000UL); }
    inline static SignalDataScalarSize  Double()  { return SignalDataScalarSize  (0x4000000000000000UL); }

    inline static SignalDataScalarType  Float()   { return SignalDataScalarType  (0x0000000000000000UL); }
    inline static SignalDataScalarType  Int()     { return SignalDataScalarType  (0x8000000000000000UL); }

    inline SignalDataType type() const { return SignalDataType(m_bits & SignalDataType::mask()); }

    inline void setScalarSize(SignalDataScalarSize sdss) { setFlagBits(sdss); }
    inline void setScalarType(SignalDataScalarType sdst) { setFlagBits(sdst); }

private:
    template<typename FlagT> inline void setFlagBits(FlagT flag)
        { m_bits &= ~FlagT::mask(); m_bits |= flag.bits(); }

public:
    inline bool isInMemory() const { return (type() & SignalDataStorageType()) == SignalDataStorage::Memory(); }

    inline bool isInRegisters() const { return type() != SignalDataStorage::Memory(); }

    inline unsigned int size() { return (m_bits & (0x0FFFFFFF00000000UL)) >> 32; }

    inline void setSize(unsigned int size)
    {
        R64FX_DEBUG_ASSERT(size <= 0x0FFFFFFF);
        m_bits &= 0xF0000000FFFFFFFFUL;
        m_bits |= ((unsigned long)size) << 32;
    }

private:
    inline void setStorageType(SignalDataStorageType sdst) { setFlagBits(sdst); }

    inline void setLowerBits(unsigned long bits) { m_bits &= 0xFFFFFFFF00000000UL; m_bits |= bits; }

    inline unsigned int lowerBits() const { return m_bits & 0x00000000FFFFFFFFUL; }

    inline void clear() { m_bits = 0; }
};

typedef SignalDataStorage SDS;


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

    template<typename MemT> inline MemT ptrMem(DataBufferPointer ptr) const { return MemT(codeBegin() - ptr.m_offset); };


    unsigned int allocGPR(GPR64* gprs, unsigned int ngprs);

    void freeGPR(GPR64* gprs, unsigned int ngprs);


    unsigned int allocXmm(Xmm* xmms, unsigned int nxmms);

    void freeXmm(Xmm* xmms, unsigned int nxmms);


    void setStorage(SignalDataStorage &storage, DataBufferPointer ptr);

    void setStorage(SignalDataStorage &storage, GPR* gprs, unsigned int ngprs);

    void getStorage(SignalDataStorage storage, GPR* gprs, unsigned int* ngprs);

    void setStorage(SignalDataStorage &storage, Xmm* xmms, unsigned int nxmms);

    void getStorage(SignalDataStorage storage, Xmm* xmms, unsigned int* nxmms);

    void freeStorage(SignalDataStorage &storage);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
