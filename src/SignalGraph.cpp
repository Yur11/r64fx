#include "SignalGraph.hpp"
#include "sleep.hpp"


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


void SignalGraph::process()
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
                node_class->process();
            }
        }

        for(auto node_class : m_node_classes)
        {
            node_class->finish();
        }
    }
    else
    {
        sleep_microseconds(100);
    }
}

}//namespace r64fx