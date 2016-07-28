#ifndef R64FX_HOST_MODEL_HPP
#define R64FX_HOST_MODEL_HPP


namespace r64fx{

class ProcessorThread;
class MachineModel;

class HostModel{
    ProcessorThread* m_processor_thread;

public:
    HostModel();

    ~HostModel();

    void deployMachine(MachineModel* machine);

    void removeMachine(MachineModel* machine);

    void pickMachine(MachineModel* machine);
};

}//namespace r64fx

#endif//R64FX_HOST_MODEL_HPP