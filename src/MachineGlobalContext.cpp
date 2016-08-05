#include "MachineGlobalContext.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
MachineGlobalContext::MachineGlobalContext()
{
//     m_sound_driver = SoundDriver::newInstance();
//     m_signal_graph = new SignalGraph(m_sound_driver);
//     m_sound_driver->enable();
}
    

MachineGlobalContext::~MachineGlobalContext()
{
//     m_sound_driver->disable();
//     delete m_signal_graph;
//     SoundDriver::deleteInstance(m_sound_driver);
}
    
    
long MachineGlobalContext::process()
{
//     if(m_signal_graph->process())
    
    return 0;
}
    
}//namespace r64fx