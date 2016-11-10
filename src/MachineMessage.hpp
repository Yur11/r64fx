#ifndef R64FX_MACHINE_MESSAGE_HPP
#define R64FX_MACHINE_MESSAGE_HPP

namespace r64fx{

struct MachineMessage{
    unsigned long opcode = 0;
    unsigned long value  = 0;

    explicit MachineMessage(unsigned long opcode, unsigned long value)
    : opcode(opcode)
    , value(value)
    {}
    
    explicit MachineMessage(unsigned long opcode, void* value)
    : opcode(opcode)
    , value((unsigned long)value)
    {}

    MachineMessage()
    {}
};


union MsgVal{
private:
    unsigned long mulong = 0;
    float mf[2];

public:
    explicit MsgVal(unsigned long ulong) : mulong(ulong) {}

    explicit MsgVal(float f0, float f1 = 0.0f) : mf{f0, f1} {}

    MsgVal() {}

    inline unsigned long ulong() const { return mulong; }

    inline operator unsigned long() const { return mulong; }

    inline float f(int i = 0) const { return mf[i]; }
};

}//namespace r64fx

#endif//R64FX_MACHINE_MESSAGE_HPP
