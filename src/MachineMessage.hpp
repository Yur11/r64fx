#ifndef R64FX_MACHINE_MESSAGE_HPP
#define R64FX_MACHINE_MESSAGE_HPP

namespace r64fx{
    
struct MachineMessage{
    unsigned long opcode = 0;
    unsigned long value  = 0;
    
    MachineMessage(unsigned long opcode, unsigned long value)
    : opcode(opcode)
    , value(value)
    {}
    
    MachineMessage()
    {}
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_MESSAGE_HPP