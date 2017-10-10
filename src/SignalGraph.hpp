#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "LinkedList.hpp"
#include "IteratorPair.hpp"
#include "jit.hpp"

namespace r64fx{

class SignalGraph;
class SignalGraphProcessor;
class SignalNode;

class SignalDataStorage{
    unsigned long m_bits = 0;

public:
    SignalDataStorage() {}

    constexpr static unsigned long Memory   = 0x0000000000000000UL;
    constexpr static unsigned long GPR      = 0x1000000000000000UL;
    constexpr static unsigned long Xmm      = 0x2000000000000000UL;
//     constexpr static unsigned long Ymm      = 0x3000000000000000UL;

    constexpr static unsigned long Single   = 0x0000000000000000UL;
    constexpr static unsigned long Double   = 0x4000000000000000UL;

    constexpr static unsigned long Float    = 0x0000000000000000UL;
    constexpr static unsigned long Int      = 0x8000000000000000UL;

    inline unsigned long type() const { return m_bits & 0xF000000000000000UL; }

    inline bool isInRegisters() const { return m_bits & 0x3000000000000000UL; }

    inline bool isInMemory() const { return !isInRegisters(); }

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

private:
    inline void setMemoryOffset(unsigned int offset)
    {
        R64FX_DEBUG_ASSERT(isInMemory());
        m_bits &= 0xFFFFFFFF00000000UL;
        m_bits |= offset;
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
        { R64FX_DEBUG_ASSERT((m_bits & 0x3000000000000000UL) == SignalDataStorage::GPR); }

    inline void checkReisterType(r64fx::Xmm*)
        { R64FX_DEBUG_ASSERT((m_bits & 0x3000000000000000UL) == SignalDataStorage::Xmm); }
/*
    inline void checkReisterType(r64fx::Ymm*)
        { R64FX_DEBUG_ASSERT((m_bits & 0x3000000000000000UL) == SignalDataStorage::Ymm); }*/
};


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource{
    friend class SignalGraph;
    friend class SignalNode;

    SignalDataStorage m_storage;
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

    inline SignalDataStorage &storage()  { return m_storage; }

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
    friend class SignalGraph;

    SignalSource*  m_connected_source  = nullptr;

public:
    SignalSink() {}

    ~SignalSink() {}

    inline SignalSource* connectedSource() const { return m_connected_source; }
};


class SignalNode : public LinkedList<SignalNode>::Node{
    friend class SignalGraph;
    friend class SignalGraphProcessor;

    SignalGraph*  m_parent_graph               = nullptr;
    unsigned int  m_incoming_connection_count  = 0;
    unsigned int  m_outgoing_connection_count  = 0;

protected:
    unsigned long m_flags = 0;

public:
    SignalNode() {}

    virtual ~SignalNode() {}

    inline SignalGraph* parentGraph() const { return m_parent_graph; }

    inline unsigned int incomingConnectionCount() const { return m_incoming_connection_count; }

    inline unsigned int outgoingConnectionCount() const { return m_outgoing_connection_count; }

private:
    virtual void getSources(SignalSource* &sources, unsigned int &nsources);

    virtual void getSinks(SignalSink* &sinks, unsigned int &nsinks);

    virtual void build(SignalGraphProcessor &sgp) = 0;
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

    inline SignalSource* sources(unsigned int i)
    {
        R64FX_DEBUG_ASSERT(i < SourceCount);
        return m_sources + i;
    }

    inline SignalSink* sinks(unsigned int i)
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

    inline SignalSource* sources(unsigned int i)
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
    inline SignalSink* sinks(unsigned int i)
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


class SignalGraph : public SignalNode_WithPorts<1, 1>{
    friend class SignalGraphProcessor;

    LinkedList<SignalNode> m_nodes;

public:
    SignalGraph() {}

    ~SignalGraph() {}

    void addNode(SignalNode* node);

    void removeNode(SignalNode* node);

    void connect(const NodeSource node_source, const NodeSink &node_sink);

    void disconnect(const NodeSink &node_sink);

    inline NodeSource linkSource() { return {this, sources(0)}; }

    inline NodeSink linkSink() { return {this, sinks(0)}; }

private:
    virtual void build(SignalGraphProcessor &sgp) override final;

    void buildNode(SignalGraphProcessor &sgp, SignalNode* node);
};


class SignalGraphProcessor{
    Assembler  m_assembler;

    float* m_data = nullptr;
    long  m_data_size = 0;

    void* m_gprs[16];
    void* m_xmms[16];

    unsigned int m_main_buffer_size = 0;

public:
    SignalGraphProcessor() {}

    ~SignalGraphProcessor() {}

    void build(SignalGraph &sg, unsigned int main_buffer_size);

    inline void run() { ((void (*)(float* data_ptr)) m_assembler.codeBegin())(m_data); }

    inline Assembler &assembler() { return m_assembler; }

    inline void* data() const { return m_data; }

    inline unsigned int mainBufferSize() const { return m_main_buffer_size; }

    void memoryStorage(SignalDataStorage* storage, int ndwords, int align_dwords);
};


}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
