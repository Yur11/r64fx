#ifndef R64FX_MACHINE_GLOBAL_CONTEXT_HPP
#define R64FX_MACHINE_GLOBAL_CONTEXT_HPP

namespace r64fx{
    
class SoundDriver;
class SignalGraph;
    
class MachineGlobalContext{
    SoundDriver* m_sound_driver = nullptr;
    SignalGraph* m_signal_graph = nullptr;
    
public:
    MachineGlobalContext();
    
    ~MachineGlobalContext();
    
    inline void setSoundDriver(SoundDriver* sd)
    {
        m_sound_driver = sd;
    }
    
    inline SoundDriver* soundDriver() const
    {
        return m_sound_driver;
    }

    inline void setSignalGraph(SignalGraph* sg)
    {
        m_signal_graph = sg;
    }
    
    inline SignalGraph* signalGraph() const
    {
        return m_signal_graph;
    }
    
    long process();
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_GLOBAL_CONTEXT_HPP