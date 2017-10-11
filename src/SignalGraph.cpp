#include "SignalGraph.hpp"

#ifdef R64FX_DEBUG
#define R64FX_BUILD_IN_PROGRESS 1UL
#endif//R64FX_DEBUG
#define R64FX_NODE_IS_BUILT     2UL

namespace r64fx{

void SignalNode::getSources(SignalSource*&, unsigned int &nsources)
{
    nsources = 0;
}


void SignalNode::getSinks(SignalSink*&, unsigned int &nsinks)
{
    nsinks = 0;
}


void SignalGraph::addNode(SignalNode* node)
{
    R64FX_DEBUG_ASSERT(node);
    R64FX_DEBUG_ASSERT(node->parentGraph() == nullptr);
    m_terminal_nodes.append(node);
    node->m_parent_graph = this;
    node->m_flags &= ~R64FX_NODE_IS_BUILT;
    node->m_flags |= m_flags & R64FX_NODE_IS_BUILT;
}


void SignalGraph::removeNode(SignalNode* node)
{
    R64FX_DEBUG_ASSERT(node);
    R64FX_DEBUG_ASSERT(node->parentGraph() == this);
    R64FX_DEBUG_ASSERT(node->m_incoming_connection_count == 0);
    R64FX_DEBUG_ASSERT(node->m_outgoing_connection_count == 0);
    m_terminal_nodes.remove(node);
    node->m_parent_graph = nullptr;
    node->m_flags = 0;
}


void SignalGraph::connect(const NodeSource node_source, const NodeSink &node_sink)
{
    auto source_node = node_source.node();
    auto sink_node = node_sink.node();
    auto source = node_source.port();
    auto sink = node_sink.port();

    R64FX_DEBUG_ASSERT(source_node);
    R64FX_DEBUG_ASSERT(sink_node);
    R64FX_DEBUG_ASSERT(source_node != sink_node);
    R64FX_DEBUG_ASSERT(source_node->parentGraph() == sink_node->parentGraph());
    R64FX_DEBUG_ASSERT(source_node->parentGraph() != nullptr);
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


void SignalGraph::disconnect(const NodeSink &node_sink)
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


void SignalGraph::build(SignalGraphProcessor &sgp)
{
    for(auto node : m_terminal_nodes)
        buildNode(sgp, node);
}


void SignalGraph::buildNode(SignalGraphProcessor &sgp, SignalNode* node)
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
                buildNode(sgp, source_node);
        }
    }
    node->build(sgp);
    node->m_flags ^= R64FX_NODE_IS_BUILT;

#ifdef R64FX_DEBUG
    node->m_flags &= ~R64FX_BUILD_IN_PROGRESS;
#endif//R64FX_DEBUG
}


void SignalGraphProcessor::build(SignalGraph &sg, unsigned int main_buffer_size)
{
    m_main_buffer_size = main_buffer_size;
    if(m_main_buffer_size == 0)
        return;

    long old_data_size = m_data_size;
    m_data_size = 0;

    m_assembler.setCodeEnd(m_assembler.codeBegin());

    m_assembler.mov(rcx, Imm32(-m_main_buffer_size));
    JumpLabel loop;
    m_assembler.mark(loop);

    sg.buildNode(*this, &sg);

    m_assembler.add(rcx, Imm32(1));
    m_assembler.jnz(loop);

    m_assembler.ret();

    if(m_data_size > old_data_size)
    {
        if(old_data_size > 0)
        {
            delete[] m_data;
        }
        m_data = new float[m_data_size];
    }
}


void SignalGraphProcessor::memoryStorage(SignalDataStorage *storage, int ndwords, int align_dwords)
{
//     while(m_data_size & (align_dwords-1))
//         m_data_size++;
//     SignalDataStorage_Memory mem_storage(m_data_size, ndwords);
//     m_data_size += ndwords;
//     storage->u.l = mem_storage.u.l;
}


void SignalNode_Dummy::build(SignalGraphProcessor &sgp)
{
    std::cout << "dummy: " << this << "\n";
}

}//namespace r64fx
