#include "SignalGraph.hpp"
#include "SignalNodeFlags.hpp"
#include <cstring>


namespace r64fx{

SignalNode* SignalNode::root()
{
     if(m_parent_subgraph)
         return m_parent_subgraph->root();
     return this;
}


SignalGraph* SignalNode::rootGraph()
{
    auto node = root();
    R64FX_DEBUG_ASSERT(dynamic_cast<SignalGraph*>(node));
    return static_cast<SignalGraph*>(node);
}


void SignalNode::getSources(SignalSource*&, unsigned int &nsources)
{
    nsources = 0;
}


void SignalNode::getSinks(SignalSink*&, unsigned int &nsinks)
{
    nsinks = 0;
}


void SignalSubGraph::addNode(SignalNode* node)
{
    R64FX_DEBUG_ASSERT(node);
    R64FX_DEBUG_ASSERT(node->parentSubGraph() == nullptr);
    m_terminal_nodes.append(node);
    node->m_parent_subgraph = this;
    node->m_flags &= ~R64FX_NODE_IS_BUILT;
    node->m_flags |= m_flags & R64FX_NODE_IS_BUILT;
}


void SignalSubGraph::removeNode(SignalNode* node)
{
    R64FX_DEBUG_ASSERT(node);
    R64FX_DEBUG_ASSERT(node->parentSubGraph() == this);
    R64FX_DEBUG_ASSERT(node->m_incoming_connection_count == 0);
    R64FX_DEBUG_ASSERT(node->m_outgoing_connection_count == 0);
    m_terminal_nodes.remove(node);
    node->m_parent_subgraph = nullptr;
    node->m_flags = 0;
}


void SignalSubGraph::connect(const NodeSource node_source, const NodeSink &node_sink)
{
    auto source_node = node_source.node();
    auto sink_node = node_sink.node();
    auto source = node_source.port();
    auto sink = node_sink.port();

    R64FX_DEBUG_ASSERT(source_node);
    R64FX_DEBUG_ASSERT(sink_node);
    R64FX_DEBUG_ASSERT(source_node != sink_node);
    R64FX_DEBUG_ASSERT(source_node->parentSubGraph() == sink_node->parentSubGraph());
    R64FX_DEBUG_ASSERT(source_node->parentSubGraph() != nullptr);
    R64FX_DEBUG_ASSERT(source_node->m_outgoing_connection_count < 0xFFFFFFFF);
    R64FX_DEBUG_ASSERT(sink_node->m_incoming_connection_count < 0xFFFFFFFF);
    R64FX_DEBUG_ASSERT(source);
    R64FX_DEBUG_ASSERT(sink);
    R64FX_DEBUG_ASSERT(sink->connectedSource() == nullptr);

    sink->m_connected_source = source;
    source->m.connected_sink_count++;
    source_node->m_outgoing_connection_count++;
    sink_node->m_incoming_connection_count++;

    if(source_node->next() || source_node->prev())
        m_terminal_nodes.remove(source_node);
}


void SignalSubGraph::disconnect(const NodeSink &node_sink)
{
    SignalSink* sink = node_sink.port();
    R64FX_DEBUG_ASSERT(sink);

    SignalNode* sink_node = node_sink.node();
    R64FX_DEBUG_ASSERT(sink_node);

    SignalSource* source = sink->m_connected_source;
    R64FX_DEBUG_ASSERT(source);

    SignalNode* source_node = source->parentNode();
    R64FX_DEBUG_ASSERT(source_node);

    R64FX_DEBUG_ASSERT(source->m.connected_sink_count > 0);
    source->m.connected_sink_count--;

    R64FX_DEBUG_ASSERT(sink_node->m_incoming_connection_count > 0);
    sink_node->m_incoming_connection_count--;

    R64FX_DEBUG_ASSERT(source_node->m_outgoing_connection_count > 0);
    source_node->m_outgoing_connection_count--;
    if(source_node->m_outgoing_connection_count == 0)
        m_terminal_nodes.append(source_node);

    sink->m_connected_source = nullptr;
}


void SignalSubGraph::build(SignalGraphCompiler &c)
{
    for(auto node : m_terminal_nodes)
    buildNode(c, node);
}


void SignalSubGraph::buildNode(SignalGraphCompiler &c, SignalNode* node)
{
#ifdef R64FX_DEBUG
    assert(!(node->m_flags & R64FX_BUILD_IN_PROGRESS));
    node->m_flags |= R64FX_BUILD_IN_PROGRESS;
#endif//R64FX_DEBUG

    if(node->incomingConnectionCount() > 0)
    {
        SignalSink* sinks = nullptr;
        unsigned int nsinks = 0;
        node->getSinks(sinks, nsinks);
        for(unsigned int i=0; i<nsinks; i++)
        {
            auto source = sinks[i].connectedSource();
            if(!source)
                continue;

            auto source_node = source->parentNode();
            if(!((source_node->m_flags & R64FX_NODE_IS_BUILT) ^ (node->m_flags & R64FX_NODE_IS_BUILT)))
                buildNode(c, source_node);
        }
    }
    node->build(c);
    node->m_flags ^= R64FX_NODE_IS_BUILT;

#ifdef R64FX_DEBUG
    node->m_flags &= ~R64FX_BUILD_IN_PROGRESS;
#endif//R64FX_DEBUG
}


unsigned char* SignalGraphCompiler::allocMemory(unsigned int nbytes, unsigned int align)
{
    R64FX_DEBUG_ASSERT(nbytes > 0);
    R64FX_DEBUG_ASSERT(align > 0);
    unsigned int align_mask = align - 1;
    R64FX_DEBUG_ASSERT((align & align_mask) == 0);

    if(dataBufferSize() == 0)
        growData(memory_page_size());

    for(;;)
    {
        HeapBuffer hb(dataBegin(), dataBufferSize());
        auto chunk = (unsigned char*) hb.allocChunk(nbytes, align);
        if(chunk)
            return chunk;

        growData(memory_page_size());
        memcpy(dataBegin(), dataBegin() + memory_page_size(), hb.headerSize());
    }
    return nullptr;
}


void SignalGraphCompiler::freeMemory(unsigned char* mem)
{
    R64FX_DEBUG_ASSERT(mem);
    HeapBuffer hb(dataBegin(), dataBufferSize());
    hb.freeChunk(mem);
}


void* SignalGraphCompiler::allocStorage(SignalDataStorage &storage, SignalDataType type, unsigned int nitems, unsigned int align)
{
    R64FX_DEBUG_ASSERT(!storage);
    if((type & SignalDataStorageType()) == SignalDataStorage::Memory())
    {
        unsigned int nbytes = nitems;
        if((type & SignalDataScalarSize()) == SignalDataStorage::Double())
            nbytes <<= 1;

        auto addr = allocMemory(nbytes, align);
        storage.setMemoryAddr(addr, *this);
        storage.setSize(nitems);
        storage.setType(type);
        return addr;
    }
    return nullptr;
}


void SignalGraphCompiler::freeStorage(SignalDataStorage &storage)
{
    R64FX_DEBUG_ASSERT(storage);
    auto type = storage.type();
    if((type & SignalDataStorageType()) == SignalDataStorage::Memory())
    {
        auto addr = storage.memoryAddr(*this);
        freeMemory(addr);
    }
    storage.clear();
}


SignalGraph::SignalGraph()
{

}


SignalGraph::~SignalGraph()
{
    
}


void SignalGraph::build()
{
    if(frameCount() == 0)
        return;

    setCodeEnd(codeBegin());

    MOV(rcx, Imm32(-frameCount()));
    JumpLabel loop;
    mark(loop);

    SignalSubGraph::build(*this);

    ADD(rcx, Imm32(1));
    JNZ(loop);

    RET();
}


void SignalNode_Dummy::build(SignalGraphCompiler &c)
{
    std::cout << "dummy: " << this << "\n";
}

}//namespace r64fx
