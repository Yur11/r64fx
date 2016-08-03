#ifndef R64FX_MACHINE_MODEL_HPP
#define R64FX_MACHINE_MODEL_HPP

namespace r64fx{

class ProcessorMessage;
class HostModelPrivate;
class MachineProcessor;
class MachineModelIface;

class MachineModel{
    friend class HostModelPrivate;
    
    HostModelPrivate* m_parent_host = nullptr;
    
    inline void setParentHost(HostModelPrivate* host)
    {
        m_parent_host = host;
    }
    
public:
    void detach();
    
protected:
    void sendMessage(const ProcessorMessage &msg);
    
    virtual void dispatchMessage(const ProcessorMessage &msg) = 0;
    
private:
    virtual MachineProcessor* processor() = 0;
};

}//namespace r64fx

#endif//R64FX_MACHINE_MODEL_HPP
