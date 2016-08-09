#include "SignalGraph.hpp"

namespace r64fx{

SignalGraphProcessable::~SignalGraphProcessable()
{
    
}
    
    
void SignalGraphProcessable::prepare()
{
    
}


void SignalGraphProcessable::processSample(int i)
{
    
}


void SignalGraphProcessable::finish()
{
    
}
    
    
void SignalConnection::processSample(int i)
{
    m_sink[0] = m_source[0];
}


void SignalGraph::addNode(SignalNode* node)
{
    m_single_nodes.append(node);
}


void SignalGraph::removeNode(SignalNode* node)
{
    m_single_nodes.remove(node);
}
    
    
void SignalGraph::addConnection(SignalConnection* connection)
{
    m_connections.append(connection);
}


void SignalGraph::prepare()
{
    for(auto item : m_single_nodes)
    {
        item->prepare();
    }
    
    for(auto item : m_node_classes)
    {
        item->prepare();
    }
    
    for(auto item : m_connections)
    {
        item->prepare();
    }
    
    for(auto item : m_subgraphs)
    {
        item->prepare();
    }
}
    
    
void SignalGraph::processSample(int i)
{
    for(auto item : m_single_nodes)
    {
        item->processSample(i);
    }
    
    for(auto item : m_node_classes)
    {
        item->processSample(i);
    }
    
    for(auto item : m_connections)
    {
        item->processSample(i);
    }
    
    for(auto item : m_subgraphs)
    {
        item->processSample(i);
    }
}


void SignalGraph::finish()
{
    for(auto item : m_single_nodes)
    {
        item->finish();
    }
    
    for(auto item : m_node_classes)
    {
        item->finish();
    }
    
    for(auto item : m_connections)
    {
        item->finish();
    }
    
    for(auto item : m_subgraphs)
    {
        item->finish();
    }
}

}//namespace r64fx