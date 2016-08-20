#ifndef R64FX_MACHINE_CONNECTION_HPP
#define R64FX_MACHINE_CONNECTION_HPP

namespace r64fx{
    
class MachineSignalSource;
class MachineSignalSink;
class MachineConnectionImpl;
    
class MachineConnection : public LinkedList<MachineConnection>::Node{
    MachineSignalSource*    m_source_port  = nullptr;
    MachineSignalSink*      m_sink_port    = nullptr;
    MachineConnectionImpl*  m_impl         = nullptr;
    
public:
    enum class Mapping{
        Default
    };
    
private:
    Mapping m_mapping = Mapping::Default;
    
public:
    MachineConnection(MachineSignalSource* source_port, MachineSignalSink* sink_port, Mapping mapping = Mapping::Default)
    : m_source_port(source_port)
    , m_sink_port(sink_port)
    , m_mapping(mapping)
    {
        
    }
    
    inline MachineSignalSource* sourcePort() const
    {
        return m_source_port;
    }
    
    inline MachineSignalSink* sinkPort() const
    {
        return m_sink_port;
    }
    
    inline void setImpl(MachineConnectionImpl* impl)
    {
        m_impl = impl;
    }
    
    inline MachineConnectionImpl* impl() const
    {
        return m_impl;
    }
    
    inline bool isReady() const
    {
        return m_impl != nullptr;
    }
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_CONNECTION_HPP