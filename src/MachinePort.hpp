#ifndef R64FX_R64FX_MACHINE_PORT_HPP
#define R64FX_R64FX_MACHINE_PORT_HPP

#include <string>
#include "LinkedList.hpp"

namespace r64fx{
    
class MachinePort : public LinkedList<MachinePort>::Node{
    void* m_handle = 0;
    std::string m_name = "";
    unsigned long m_flags = 0;
    
public:
    MachinePort(const std::string &name, bool is_input, bool is_signal)
    {
        setName(name);
        isInput(is_input);
        isSignalPort(is_signal);
    }
    
    inline void setHandle(void* handle)
    {
        m_handle = handle;
    }
    
    inline void* handle() const
    {
        return m_handle;
    }
    
    inline void setName(const std::string &name)
    {
        m_name = name;
    }
    
    inline std::string name() const
    {
        return m_name;
    }
    
    inline bool isInput(bool yes)
    {
        if(yes)
            m_flags |= 1;
        else
            m_flags &= ~1;
        return yes;
    }
    
    inline bool isInput() const
    {
        return m_flags & 1;
    }
    
    inline bool isOutput(bool yes)
    {
        return isInput(!yes);
    }
    
    inline bool isOutput() const
    {
        return !isInput();
    }
    
    inline bool isSignalPort(bool yes)
    {
        if(yes)
            m_flags |= 2;
        else
            m_flags &= ~2;
        return yes;
    }
    
    inline bool isSignalPort() const
    {
        return m_flags & 2;
    }
    
    inline bool isSequncerPort(bool yes)
    {
        return isSignalPort(!yes);
    }
    
    inline bool isSequncerPort() const
    {
        return !isSequncerPort();
    }
};
    
}//namespace r64fx

#endif//R64FX_R64FX_MACHINE_PORT_HPP