#include "MachinePoolContext.hpp"
#include "MachineImpl.hpp"

namespace r64fx{
    
    
MachinePoolContext::MachinePoolContext()
{
    sound_driver = SoundDriver::newInstance(SoundDriver::Type::Stub);
    status_port = sound_driver->newStatusPort();
    
    root_graph = new SignalGraph;
    input_subgraph = new SignalGraph;
    main_subgraph = new SignalGraph;
    output_subgraph = new SignalGraph;
    root_graph->addItem(input_subgraph);
    root_graph->addItem(main_subgraph);
    root_graph->addItem(output_subgraph);
}
    
    
MachinePoolContext::~MachinePoolContext()
{
    if(root_graph)
    {
        root_graph->clear();
        if(input_subgraph)
        {
            input_subgraph->clear();
            delete input_subgraph;
        }
        
        if(main_subgraph)
        {
            main_subgraph->clear();
            delete main_subgraph;
        }
        
        if(output_subgraph)
        {
            output_subgraph->clear();
            delete output_subgraph;
        }
        delete root_graph;
    }
    
    if(sound_driver)
    {
        SoundDriver::deleteInstance(sound_driver);
    }
}
 
        
long MachinePoolContext::process()
{
    SoundDriverIOStatus status;
    if(status_port->readStatus(&status, 1))
    {
        if(root_graph)
        {
            root_graph->prepare();
            for(int i=0; i<sound_driver->bufferSize(); i++)
            {
                root_graph->processSample(i);
            }
            root_graph->finish();
        }
    }
    else
    {
    }
    
    return 0;
}
    
}//namespace r64fx
