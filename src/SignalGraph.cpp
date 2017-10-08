#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

#define R64FX_CALLEE_PRESERVED_GPR_MASK (int(rbp.code() | rbx.code() | r12.code() | r13.code() | r14.code() | r15.code()))

namespace r64fx{

template<typename RegT, int MaxRegCount> inline void get_registers(long bits, RegT* regs, int* nregs)
{
    bits &= (MaxRegCount - 1);
    int n = 0;
    for(int i=0; bits && i<16; i++)
    {
        long b = (1 << i);
        if(bits & b)
        {
            regs[n++] = i;
            bits &= ~b;
        }
    }
    *nregs = n;
}



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
    m_nodes.append(node);
    node->m_parent_graph = this;
}


void SignalGraph::removeNode(SignalNode* node)
{
    R64FX_DEBUG_ASSERT(node);
    R64FX_DEBUG_ASSERT(node->parentGraph() == this);
    R64FX_DEBUG_ASSERT(node->m_incoming_connection_count == 0);
    R64FX_DEBUG_ASSERT(node->m_outgoing_connection_count == 0);
    m_nodes.remove(node);
    node->m_parent_graph = nullptr;
}


void SignalGraph::connect(const NodeSource node_source, const NodeSink &node_sink)
{
    auto source_node = node_source.node();
    auto sink_node = node_sink.node();
    auto source = node_source.port();
    auto sink = node_sink.port();

    R64FX_DEBUG_ASSERT(source_node);
    R64FX_DEBUG_ASSERT(sink_node);
    R64FX_DEBUG_ASSERT(source);
    R64FX_DEBUG_ASSERT(sink);
    R64FX_DEBUG_ASSERT(source_node->m_outgoing_connection_count < 0xFFFFFFFF);
    R64FX_DEBUG_ASSERT(sink_node->m_incoming_connection_count < 0xFFFFFFFF);

    sink->m_connected_source = source;
    sink->m_connected_node = source_node;
    source->m_outgoing_connection_count++;
    source_node->m_outgoing_connection_count++;
    sink_node->m_incoming_connection_count++;

    if(source_node->next() || source_node->prev())
        m_nodes.remove(source_node);
}


void SignalGraph::disconnect(const NodeSink &node_sink)
{
    SignalSink* sink = node_sink.port();
    R64FX_DEBUG_ASSERT(sink);

    SignalSource* source = sink->m_connected_source;
    R64FX_DEBUG_ASSERT(source);

    SignalNode* sink_node = node_sink.node();
    R64FX_DEBUG_ASSERT(sink_node);

    SignalNode* source_node = sink->m_connected_node;
    R64FX_DEBUG_ASSERT(source_node);

    R64FX_DEBUG_ASSERT(source->m_outgoing_connection_count > 0);
    source->m_outgoing_connection_count--;

    R64FX_DEBUG_ASSERT(sink_node->m_incoming_connection_count > 0);
    sink_node->m_incoming_connection_count--;

    R64FX_DEBUG_ASSERT(source_node->m_outgoing_connection_count > 0);
    source_node->m_outgoing_connection_count--;
    if(source_node->m_outgoing_connection_count == 0)
        m_nodes.append(source_node);

    sink->m_connected_source = nullptr;
    sink->m_connected_node = nullptr;
}


void SignalGraph::build(SignalGraphProcessor &sgp)
{
    for(auto node : m_nodes)
        buildNode(sgp, node);
}


void SignalGraph::buildNode(SignalGraphProcessor &sgp, SignalNode* node)
{
    if(node->incomingConnectionCount() > 0)
    {
        SignalSink* sinks = nullptr;
        unsigned int nsinks = 0;
        node->getSinks(sinks, nsinks);
        for(unsigned int i=0; i<nsinks; i++)
        {
            auto connected_node = sinks[i].connectedNode();
            if(connected_node)
                buildNode(sgp, connected_node);
        }
    }
    node->build(sgp);
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

    sg.build(*this);

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
    while(m_data_size & (align_dwords-1))
        m_data_size++;
    SignalDataStorage_Memory mem_storage(m_data_size, ndwords);
    m_data_size += ndwords;
    storage->u.l = mem_storage.u.l;
}

}//namespace r64fx
