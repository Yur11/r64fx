#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

SignalGraphProcessable::~SignalGraphProcessable()
{
    removeFromGraph();
}


void SignalGraphProcessable::addedToGraph(SignalGraph* graph)
{
    
}


void SignalGraphProcessable::aboutToBeRemovedFromGraph(SignalGraph* graph)
{
    
}


void SignalGraphProcessable::removeFromGraph()
{
    if(m_graph)
    {
        m_graph->removeItem(this);
    }
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


SignalConnection::SignalConnection(SignalSource* source, SignalSink* sink)
: m_source(source)
, m_sink(sink)
{
    
}


void SignalConnection::setSource(SignalSource* source)
{
    m_source = source;
}
    
    
SignalSource* SignalConnection::source() const
{
    return m_source;
}


void SignalConnection::setSink(SignalSink* sink)
{
    m_sink = sink;
}


SignalSink* SignalConnection::sink() const
{
    return m_sink;
}
    
    
void SignalConnection::processSample(int i)
{
//     cout << i << " connection\n";
    m_sink[0][0] = m_source[0][0];
}


void SignalGraph::addItem(SignalGraphProcessable* item)
{
    if(item->graph() != this)
    {
        m_items.append(item);
        item->setGraph(this);
        item->addedToGraph(this);
    }
}
    
    
void SignalGraph::removeItem(SignalGraphProcessable* item)
{
    if(item->graph() == this)
    {
        item->aboutToBeRemovedFromGraph(this);
        item->setGraph(nullptr);
        m_items.remove(item);
    }
}


void SignalGraph::clear()
{
    while(m_items.last())
        removeItem(m_items.last());
}


void SignalGraph::replaceSource(SignalSource* old_source, SignalSource* new_source)
{
    for(auto item : m_items)
    {
        auto signal_connection = dynamic_cast<SignalConnection*>(item);
        if(signal_connection)
        {
            if(signal_connection->source() == old_source)
            {
                signal_connection->setSource(new_source);
            }
        }
        else
        {
            auto signal_graph = dynamic_cast<SignalGraph*>(item);
            {
                signal_graph->replaceSource(old_source, new_source);
            }
        }
    }
}


void SignalGraph::replaceSink(SignalSink* old_sink, SignalSink* new_sink)
{
    for(auto item : m_items)
    {
        auto signal_connection = dynamic_cast<SignalConnection*>(item);
        if(signal_connection)
        {
            if(signal_connection->sink() == old_sink)
            {
                signal_connection->setSink(new_sink);
            }
        }
        else
        {
            auto signal_graph = dynamic_cast<SignalGraph*>(item);
            {
                signal_graph->replaceSink(old_sink, new_sink);
            }
        }
    }
}


void SignalGraph::prepare()
{
    for(auto item : m_items)
    {
        item->prepare();
    }
}
    
    
void SignalGraph::processSample(int i)
{
    for(auto item : m_items)
    {
        item->processSample(i);
    }
}


void SignalGraph::finish()
{
    for(auto item : m_items)
    {
        item->finish();
    }
}

}//namespace r64fx