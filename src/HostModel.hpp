#ifndef R64FX_HOST_MODEL_HPP
#define R64FX_HOST_MODEL_HPP

#include "LinkedList.hpp"
#include "ProcessorMessage.hpp"

namespace r64fx{

class ProcessorThread;
class MachineModel;

class HostModelPrivate;

class HostModel{
    HostModelPrivate* m = nullptr;
    
    ProcessorThread* m_processor_thread;
    LinkedList<MachineModel> m_machines;

public:
    HostModel();

    ~HostModel();

    void deployMachine(MachineModel* machine);

    void removeMachine(MachineModel* machine);

    void pickMachine(MachineModel* machine);

private:
    void dispatchMessage(ProcessorMessage msg);
};

}//namespace r64fx

#endif//R64FX_HOST_MODEL_HPP