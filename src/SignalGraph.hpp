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
    friend class SignalGraphProcessor;

protected:
    union{
        unsigned long l = 0;
        unsigned int i[2];
    }u;

public:
    SignalDataStorage() {}

    inline unsigned long l() const { return u.l; }

    inline bool isNull() const { return u.l == 0; }

    inline bool isMemoryStorage() const { return !isRegisterStorage(); }

    inline bool isRegisterStorage() const { return u.l & (1UL<<63); }
};

class SignalDataStorage_Memory : public SignalDataStorage{
    friend class SignalGraphProcessor;

    SignalDataStorage_Memory(int index, int size)
    {
        setIndex(index);
        setSize(size);
    }

public:
    SignalDataStorage_Memory() {}
    SignalDataStorage_Memory(const SignalDataStorage &sds)
    {
        R64FX_DEBUG_ASSERT(sds.isMemoryStorage());
        u.l = sds.l();
    };

    inline int index() const { return u.i[0] & ~(1<<31); }
    inline int size()  const { return u.i[1] & ~(1<<31); }

    inline constexpr static int maxValue() { return ~(1<<31); }

private:
    inline void setIndex(int index)
    {
        R64FX_DEBUG_ASSERT(index <= maxValue());
        u.i[0] = index;
    }

    inline void setSize(int size)
    {
        R64FX_DEBUG_ASSERT(size <= maxValue());
        u.i[1] = size;
    }
};


namespace RegisterStorageType{
    constexpr unsigned long GPR32         = 0;
    constexpr unsigned long GPR32_Stereo  = 1;
    constexpr unsigned long GPR64         = 2;
    constexpr unsigned long Xmm           = 3;
    constexpr unsigned long Ymm           = 4;
};

class SignalDataStorage_Registers : public SignalDataStorage{
    friend class SignalGraphProcessor;

public:
    SignalDataStorage_Registers() { u.l |= (1UL<<63); }
    SignalDataStorage_Registers(const SignalDataStorage &sds)
    {
        R64FX_DEBUG_ASSERT(sds.isRegisterStorage());
        u.l = sds.l();
    }

    inline unsigned long type() { return (u.l >> 59) & 7; }

private:
    inline void setType(unsigned long type)
    {
        R64FX_DEBUG_ASSERT(type >= 0 && type <= 4);
        u.l &= ~(7UL << 59);
        u.l |= (type << 59);
    }
};


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource{
    friend class SignalGraph;

    SignalDataStorage  m_storage;
    unsigned int       m_outgoing_connection_count = 0;
    unsigned int       m_processed_sink_count = 0;

public:
    SignalSource() {}

    ~SignalSource() {}

    inline SignalDataStorage* storage() { return &m_storage; }

    inline int outgoingConnectionCount() const { return m_outgoing_connection_count; }

    inline int processedSinkCount() const { return m_processed_sink_count; }
};


/* Each SignalSink can be connected to only one SignalSource. */
class SignalSink{
    friend class SignalGraph;

    SignalSource*  m_connected_source  = nullptr;
    SignalNode*    m_connected_node    = nullptr;

public:
    SignalSink() {}

    ~SignalSink() {}

    inline SignalSource* connectedSource() const { return m_connected_source; }

    inline SignalNode* connectedNode() const { return m_connected_node; }
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

    inline NodeSource linkSource() { return {this, source(0)}; }

    inline NodeSink linkSink() { return {this, sink(0)}; }

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
