#ifndef R64FX_PROCESSOR_THREAD_CONTEXT_HPP
#define R64FX_PROCESSOR_THREAD_CONTEXT_HPP

namespace r64fx{

class ProcessorMessage;
    
class MachineProcessor{
    MachineModel* m_model = nullptr;
    
public:
    MachineProcessor(MachineModel* model)
    : m_model(model)
    {}
    
    virtual ~MachineProcessor(){}
    
    inline MachineModel* model() const
    {
        return m_model;
    }
    
    virtual void deploy() = 0;
    
    virtual void detach() = 0;
    
    virtual void dispatchMessage(const ProcessorMessage &msg) = 0;
    
protected:
    void sendMessage(const ProcessorMessage &msg);
};

}//namespace r64fx

#endif //R64FX_PROCESSOR_THREAD_CONTEXT_HPP