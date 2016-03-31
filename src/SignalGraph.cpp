#include "SignalGraph.hpp"


namespace r64fx{

SignalGraph::SignalGraph(SoundDriver* driver)
{
    if(!driver)
        return;

    m_driver = driver;
    m_status_port = m_driver->newStatusPort();
    m_buffer_size = m_driver->bufferSize();
    m_sample_rate = m_driver->sampleRate();
    m_sample_rate_rcp = 1.0f / m_sample_rate;
}


SoundDriver* SignalGraph::soundDriver() const
{
    return m_driver;
}


void SignalGraph::addNodeClass(SignalNodeClass* node_class)
{
    node_class->setParentGraph(this);
}


SignalNodeConnection* SignalGraph::newConnection(SignalNode* dst, SignalSink* dst_port, SignalNode* src, SignalPort* src_port)
{
    auto connection = new(std::nothrow) SignalNodeConnection(
        dst, dst_port, src, src_port
    );
    if(!connection)
        return nullptr;

    m_connections.append(connection);
    return connection;
}


bool SignalGraph::process()
{
    SoundDriverIOStatus status;
    while(m_status_port->readStatus(&status, 1));

    if(status)
    {
        for(auto node_class : m_node_classes)
        {
            node_class->prepare();
        }

        for(int i=0; i<m_driver->bufferSize(); i++)
        {
            for(auto node_class : m_node_classes)
            {
                node_class->process(i);
            }

            for(auto connection : m_connections)
            {
                connection->process();
            }
        }

        for(auto node_class : m_node_classes)
        {
            node_class->finish();
        }

        return true;
    }

    return false;
}


int SignalGraph::bufferSize() const
{
    return m_buffer_size;
}


float SignalGraph::sampleRate() const
{
    return m_sample_rate;
}


float SignalGraph::sampleRateReciprocal() const
{
    return m_sample_rate_rcp;
}

}//namespace r64fx