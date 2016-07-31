#ifndef R64FX_MACHINE_MODEL_HPP
#define R64FX_MACHINE_MODEL_HPP

#include "ProcessorMessage.hpp"

namespace r64fx{

class HostModel;
class MachineProcessor;

class MachineModel{
    HostModel* m_host = nullptr;

public:
    MachineModel(HostModel* host);

    virtual MachineProcessor* processor() = 0;

    virtual void dispatchMessage(ProcessorMessage msg) = 0;
    
    void sendMessageToProcessor(ProcessorMessage msg);
};

}//namespace r64fx

#endif//R64FX_MACHINE_MODEL_HPP
