#include "MachineGlobalContext.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
MachineGlobalContext::MachineGlobalContext()
{

}
    

MachineGlobalContext::~MachineGlobalContext()
{
    
}
    
    
long MachineGlobalContext::process()
{
    static SoundDriverIOStatusPort* status_port = nullptr;
    static int count = 0;
    if(m_sound_driver)
    {
        if(status_port)
        {
            SoundDriverIOStatus status;
            if(status_port->readStatus(&status, 1))
            {
                long diff = status.end_time - status.begin_time;
                cout << "process: " << count << "\n";
                count = 0;
            }
            else
            {
                count++;
            }
        }
        else
        {
            status_port = m_sound_driver->newStatusPort();
        }
    }
    
    return 0;
}
    
}//namespace r64fx