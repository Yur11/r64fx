#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "FlagUtils.hpp"
#include "LinkedList.hpp"
#include "jit.hpp"

namespace r64fx{

class SignalNode;
class SignalGraph;
class SignalSubGraph;
class SignalGraphCompiler;


class SignalDataType          : R64FX_FLAG_TYPE(0xF000000000000000UL);

/* Basic constituents of SignalDataType. */
class SignalDataStorageType   : R64FX_FLAG_TYPE(0x3000000000000000UL); //Memory, GPR or Xmm
class SignalDataScalarSize    : R64FX_FLAG_TYPE(0x4000000000000000UL); //Single or Double
class SignalDataScalarType    : R64FX_FLAG_TYPE(0x8000000000000000UL); //Float or Int

R64FX_COMBINE_3_FLAG_TYPES(SignalDataType,  SignalDataStorageType, SignalDataScalarSize, SignalDataScalarType);


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

    inline static SignalDataType        Addr()    { return SignalDataStorage::Memory() | SignalDataStorage::Double() | SignalDataStorage::Int(); }

    inline SignalDataType type() const { return SignalDataType(m_bits & SignalDataType::mask()); }

private:
    inline void setType(SignalDataType type)
    {
        m_bits &= type.mask();
        m_bits |= type.bits();
    }

public:
    inline bool isInMemory() const { return type() == SignalDataStorage::Memory(); }

    inline bool isInRegisters() const { return type() != SignalDataStorage::Memory(); }

    inline unsigned int size() { return (m_bits & (0x0FFFFFFF00000000UL)) >> 32; }

private:
    inline void setSize(unsigned int size)
    {
        R64FX_DEBUG_ASSERT(size <= 0x0FFFFFFF);
        m_bits &= 0xF0000000FFFFFFFFUL;
        m_bits |= ((unsigned long)size) << 32;
    }

public:
    inline unsigned int memoryOffset() const
    {
        R64FX_DEBUG_ASSERT(isInMemory());
        return m_bits & 0x00000000FFFFFFFFUL;
    }

    inline unsigned char* memoryAddr(const Assembler &as) const
    {
        return as.codeBegin() - memoryOffset();
    }

private:
    inline void setMemoryOffset(unsigned int offset)
    {
        R64FX_DEBUG_ASSERT(isInMemory());
        m_bits &= 0xFFFFFFFF00000000UL;
        m_bits |= offset;
    }

    inline void setMemoryAddr(unsigned char* addr, const Assembler &as)
    {
        long offset = long(as.codeBegin()) - long(addr);
        R64FX_DEBUG_ASSERT(offset > 0 && offset <= 0xFFFFFFFF);
        setMemoryOffset(offset);
    }

public:
    template<typename RegT> void listRegisters(RegT* regs, int* nregs)
    {
        checkRegisterType(regs);
        unsigned long bits = m_bits & 0xFF;
        int mask = 1;
        for(int i=0; bits && i<16; i++)
        {
            if(m_bits & mask)
            {
                regs[(*nregs)++] = regs[i];
                bits &= ~mask;
                mask <<= 1;
            }
        }
    }

private:
    inline void checkRegisterType(r64fx::GPR64*)
        { R64FX_DEBUG_ASSERT((m_bits & SignalDataStorageType::mask()) == SignalDataStorage::GPR()); }

    inline void checkRegisterType(r64fx::Xmm*)
        { R64FX_DEBUG_ASSERT((m_bits & SignalDataStorageType::mask()) == SignalDataStorage::Xmm()); }

public:
    inline void clear() { m_bits = 0; }
};

typedef SignalDataStorage SDS;


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource : public SignalDataStorage{
    friend class SignalSubGraph;
    friend class SignalNode;

    struct{
        unsigned long parent_offset        :22;
        unsigned long connected_sink_count :21;
        unsigned long processed_sink_count :21;
    }m = {0, 0, 0};

    inline static long maxParentOffset() { return (1<<22) - 1; }

    inline static long maxSinkCount() { return (1<<22) - 1; }

public:
    SignalSource() {}

    ~SignalSource() {}

    inline int connectedSinkCount() const { return m.connected_sink_count; }

    inline int processedSinkCount() const { return m.processed_sink_count; }

    inline SignalNode* parentNode() const
    {
        return (SignalNode*)(long(this) - long(m.parent_offset << 4));
    }

public:
    inline void setParentNode(SignalNode* node)
    {
        long offset = long(this) - long(node);
        R64FX_DEBUG_ASSERT(offset > 0);
        R64FX_DEBUG_ASSERT((offset & 0xF) == 0);
        offset >>= 4;
        R64FX_DEBUG_ASSERT(offset <= maxParentOffset());
        m.parent_offset = offset;
    }
};


/* Each SignalSink can be connected to only one SignalSource. */
class SignalSink{
    friend class SignalSubGraph;

    SignalSource*  m_connected_source  = nullptr;

public:
    SignalSink() {}

    ~SignalSink() {}

    inline SignalSource* connectedSource() const { return m_connected_source; }
};


class SignalNode : public LinkedList<SignalNode>::Node{
    friend class SignalSubGraph;

    SignalSubGraph*  m_parent_subgraph            = nullptr;
    unsigned int     m_incoming_connection_count  = 0;
    unsigned int     m_outgoing_connection_count  = 0;

protected:
    unsigned long    m_flags = 0;

public:
    SignalNode() {}

    virtual ~SignalNode() {}

    inline SignalSubGraph* parentSubGraph() const { return m_parent_subgraph; }

    inline unsigned int incomingConnectionCount() const { return m_incoming_connection_count; }

    inline unsigned int outgoingConnectionCount() const { return m_outgoing_connection_count; }

protected:
    SignalNode* root();

    SignalGraph* rootGraph();

private:
    virtual void getSources(SignalSource* &sources, unsigned int &nsources);

    virtual void getSinks(SignalSink* &sinks, unsigned int &nsinks);

    virtual void build(SignalGraphCompiler &c) = 0;
};


template<unsigned int SourceCount, unsigned int SinkCount> class SignalNode_WithPorts : public SignalNode{
    SignalSource  m_sources [SourceCount];
    SignalSink    m_sinks   [SinkCount];

protected:
    SignalNode_WithPorts()
    {
        for(unsigned int i=0; i<SourceCount; i++)
            m_sources[i].setParentNode(this);
    }

    inline SignalSource* source(unsigned int i)
    {
        R64FX_DEBUG_ASSERT(i < SourceCount);
        return m_sources + i;
    }

    inline SignalSink* sink(unsigned int i)
    {
        R64FX_DEBUG_ASSERT(i < SinkCount);
        return m_sinks + i;
    }

    inline unsigned int sourceCount() const { return SourceCount; }

    inline unsigned int sinkCount() const { return SinkCount; }

private:
    virtual void getSources(SignalSource* &sources, unsigned int &nsources) override final
    {
        sources = m_sources;
        nsources = SourceCount;
    }

    virtual void getSinks(SignalSink* &sinks, unsigned int &nsinks) override final
    {
        sinks = m_sinks;
        nsinks = SinkCount;
    }
};


template<unsigned int SourceCount> class SignalNode_WithSources : public SignalNode{
    SignalSource m_sources[SourceCount];

protected:
    SignalNode_WithSources()
    {
        for(unsigned int i=0; i<SourceCount; i++)
            m_sources[i].setParentNode(this);
    }

    inline SignalSource* source(unsigned int i)
    {
        R64FX_DEBUG_ASSERT(i < SourceCount);
        return m_sources + i;
    }

    inline unsigned int sourceCount() const { return SourceCount; }

private:
    virtual void getSources(SignalSource* &sources, unsigned int &nsources) override final
    {
        sources = m_sources;
        nsources = SourceCount;
    }
};


template<unsigned int SinkCount> class SignalNode_WithSinks : public SignalNode{
    SignalSink m_sinks[SinkCount];

protected:
    inline SignalSink* sink(unsigned int i)
    {
        R64FX_DEBUG_ASSERT(i < SinkCount);
        return m_sinks + i;
    }

    inline unsigned int sinkCount() const { return SinkCount; }

public:
    virtual void getSinks(SignalSink* &sinks, unsigned int &nsinks) override final
    {
        sinks = m_sinks;
        nsinks = SinkCount;
    }
};


/* Decoration class that packs both Node and SignalSource/Signal instances.
   To be used with SignalSubGraph::connect() method. */
template<typename PortT> class NodePort{
    SignalNode* m_node = nullptr;
    PortT*      m_port = nullptr;

public:
    NodePort(SignalNode* node, PortT* port)
    : m_node(node), m_port(port) {}

    inline SignalNode* node() const { return m_node; }

    inline PortT* port() const { return m_port; }
};

typedef NodePort<SignalSource>  NodeSource;
typedef NodePort<SignalSink>    NodeSink;


/* Collection of potentially connected nodes. */
class SignalSubGraph : public SignalNode_WithPorts<1, 1>{
    friend class SignalGraph;

    LinkedList<SignalNode> m_terminal_nodes;

public:
    SignalSubGraph() {}

    ~SignalSubGraph() {}

    void addNode(SignalNode* node);

    void removeNode(SignalNode* node);

    void connect(const NodeSource node_source, const NodeSink &node_sink);

    void disconnect(const NodeSink &node_sink);

    inline NodeSource linkSource() { return {this, source(0)}; }

    inline NodeSink linkSink() { return {this, sink(0)}; }

private:
    virtual void build(SignalGraphCompiler &c) override final;

    void buildNode(SignalGraphCompiler &c, SignalNode* node);
};


class SignalGraphCompiler : public Assembler{
    unsigned long  m_gprs[16];
    unsigned long  m_frame_count  = 0;

public:
    inline unsigned long frameCount() const { return m_frame_count; }

protected:
    inline void setFrameCount(unsigned long frame_count) { m_frame_count = frame_count; }

public:
    unsigned char* allocMemory(unsigned int nbytes, unsigned int align);

    void freeMemory(unsigned char* mem);

    void* allocStorage(SignalDataStorage &storage, SignalDataType type, unsigned int nitems, unsigned int align);

    void freeStorage(SignalDataStorage &storage);
};


class SignalGraph : public SignalSubGraph, private SignalGraphCompiler{

public:
    SignalGraph();

    ~SignalGraph();

    inline void setFrameCount(unsigned long frame_count) { SignalGraphCompiler::setFrameCount(frame_count); }

    inline unsigned long frameCount() const { return SignalGraphCompiler::frameCount();    }

    void build();

    inline long run() { return ((long (*)()) codeBegin())(); }
};


class SignalNode_Dummy : public SignalNode_WithPorts<1, 2>{
public:
    inline NodeSource out() { return {this, source(0)}; }

    inline NodeSink left() { return {this, sink(0)}; }

    inline NodeSink right() { return {this, sink(1)}; }

private:
    virtual void build(SignalGraphCompiler &cg) override final;
};


}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
