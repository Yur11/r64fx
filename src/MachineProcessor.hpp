#ifndef R64FX_PROCESSOR_THREAD_CONTEXT_HPP
#define R64FX_PROCESSOR_THREAD_CONTEXT_HPP

#include "LinkedList.hpp"
#include "ProcessorMessage.hpp"

namespace r64fx{

class ProcessorThreadContext;


class MachineProcessor : public LinkedList<MachineProcessor>::Node{
    MachineModel* m_model = nullptr;

public:
    MachineProcessor(MachineModel* model)
    : m_model(model)
    {}

    inline MachineModel* model() const { return m_model; }

    virtual void deploy(ProcessorThreadContext* ctx) = 0;

    virtual void remove(ProcessorThreadContext* ctx) = 0;

    virtual void dispatchMessage(ProcessorThreadContext* ctx, ProcessorMessage msg) = 0;
    
    void sendMessageToModel(ProcessorMessage msg);
};


struct ProcessorThreadContext{
    LinkedList<MachineProcessor> machines;
};

}//namespace r64fx

#endif //R64FX_PROCESSOR_THREAD_CONTEXT_HPP