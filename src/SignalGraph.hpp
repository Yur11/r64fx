#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "LinkedList.hpp"
#include "IteratorPair.hpp"
#include "jit.hpp"

namespace r64fx{

class SignalGraph;
class SignalGraphProcessor;

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

public:
    SignalDataStorage_Memory() {}
    SignalDataStorage_Memory(const SignalDataStorage &sds)
    {
        R64FX_DEBUG_ASSERT(sds.isMemoryStorage());
        u.l = sds.l();
    };

    inline int index() const { return u.i[0] & ~(1<<31); }
    inline int size()  const { return u.i[1] & ~(1<<31); }

    inline static int maxValue() { return ~(1<<31); }
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

    inline bool isGPR() const { return !isXmm(); }

    inline bool isXmm() const { return u.l & (1UL<<62); }
};

class SignalDataStorage_GPR : public SignalDataStorage_Registers{
public:
    SignalDataStorage_GPR() {}
    SignalDataStorage_GPR(const SignalDataStorage_Registers &sdsr)
    {
        R64FX_DEBUG_ASSERT(sdsr.isGPR());
        u.l = sdsr.l();
    }
};

class SignalDataStorage_Xmm : public SignalDataStorage_Registers{
    friend class SignalGraphProcessor;

public:
    SignalDataStorage_Xmm() { u.l |= (1UL<<62); }
    SignalDataStorage_Xmm(const SignalDataStorage_Registers &sdsr)
    {
        R64FX_DEBUG_ASSERT(sdsr.isXmm());
        u.l = sdsr.l();
    }

    void getRegisters(Xmm* regs, int* nregs);
};


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource{
    friend class SignalGraphProcessor;

    SignalDataStorage m_storage;
    union{
        unsigned long l = 0;
        int i[2];
    }m;

public:
    SignalSource();

    ~SignalSource();

    inline SignalDataStorage* storage() { return &m_storage; }
};


/* Each SignalSink can be connected to only one SignalSource. */
class SignalSink{
    friend class SignalGraphProcessor;

    SignalSource* m_connected_source = nullptr;

public:
    SignalSink();

    ~SignalSink();

    inline SignalSource* connectedSource() const { return m_connected_source; }
};


class SignalNode : public LinkedList<SignalNode>::Node{
    friend class SignalGraph;
    friend class SignalGraphProcessor;

    SignalGraph* m_parent_graph = nullptr;

public:
    SignalNode();

    virtual ~SignalNode();

    inline SignalGraph* parentGraph() const { return m_parent_graph; }

private:
    virtual void getSources(SignalSource* &sources, int &nsources);

    virtual void getSinks(SignalSink* &sinks, int &nsinks);

    virtual void build(SignalGraphProcessor &sgp) = 0;
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


template<int SourceCount, int SinkCount> class SignalNode_WithPorts : public SignalNode{
    SignalSource  m_sources [SourceCount];
    SignalSink    m_sinks   [SinkCount];

protected:
    inline SignalSource* source(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SourceCount);
#endif//R64FX_DEBUG
        return m_sources + i;
    }

    inline SignalSink* sink(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SinkCount);
#endif//R64FX_DEBUG
        return m_sinks + i;
    }

    inline int sourceCount() const { return SourceCount; }

    inline int sinkCount() const { return SinkCount; }

private:
    virtual void getSources(SignalSource* &sources, int &nsources) override final
    {
        sources = m_sources;
        nsources = SourceCount;
    }

    virtual void getSinks(SignalSink* &sinks, int &nsinks) override final
    {
        sinks = m_sinks;
        nsinks = SinkCount;
    }
};


template<int SourceCount> class SignalNode_WithSources : public SignalNode{
    SignalSource m_sources[SourceCount];

protected:
    inline SignalSource* source(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SourceCount);
#endif//R64FX_DEBUG
        return m_sources + i;
    }

    inline int sourceCount() const { return SourceCount; }

private:
    virtual void getSources(SignalSource* &sources, int &nsources) override final
    {
        sources = m_sources;
        nsources = SourceCount;
    }
};


template<int SinkCount> class SignalNode_WithSinks : public SignalNode{
    SignalSink m_sinks[SinkCount];

protected:
    inline SignalSink* sink(int i)
    {
#ifdef R64FX_DEBUG
        assert(i >= 0);
        assert(i < SinkCount);
#endif//R64FX_DEBUG
        return m_sinks + i;
    }

    inline int sinkCount() const { return SinkCount; }

public:
    virtual void getSinks(SignalSink* &sinks, int &nsinks) override final
    {
        sinks = m_sinks;
        nsinks = SinkCount;
    }
};


class SignalGraph : public SignalNode{
    friend class SignalGraphProcessor;

    LinkedList<SignalNode> m_nodes;
    int m_frame_count = 0;

public:
    SignalGraph();

    ~SignalGraph();

    void addNode(SignalNode* node);

    void removeNode(SignalNode* node);

    inline void setFrameCount(int frame_count) { m_frame_count = frame_count; }

    inline int frameCount() const { return m_frame_count; }

private:
    virtual void build(SignalGraphProcessor &sgp) override final;
};


class SignalGraphProcessor{
    Assembler  m_assembler;

    void* m_data = nullptr;

public:
    SignalGraphProcessor() {}

    ~SignalGraphProcessor() {}

    inline Assembler &assembler() { return m_assembler; }

    inline void* data() const { return m_data; }

    void build(SignalGraph &sg);

    inline void run() { ((void (*)()) m_assembler.codeBegin())(); }
};


}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
