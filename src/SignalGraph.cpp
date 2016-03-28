#include "SignalGraph.hpp"


namespace r64fx{

SignalGraph::SignalGraph(SoundDriver* driver)
{
    if(!driver)
        return;

    m_driver = driver;
    m_status_port = m_driver->newStatusPort();
}


void SignalGraph::addNodeClass(SignalNodeClass* node_class)
{
    m_node_classes.append(node_class);
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
        }

        for(auto node_class : m_node_classes)
        {
            node_class->finish();
        }

        return true;
    }

    return false;
}

}//namespace r64fx