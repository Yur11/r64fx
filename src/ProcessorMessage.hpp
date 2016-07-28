#ifndef R64FX_PROCESSOR_MESSAGE_HPP
#define R64FX_PROCESSOR_MESSAGE_HPP

namespace r64fx{

struct ProcessorMessage{
    unsigned long type = 0;
    unsigned long param = 0;

    ProcessorMessage(unsigned long type, unsigned long param = 0)
    : type(type)
    , param(param)
    {}

    ProcessorMessage() {}
};

}//namespace r64fx

#endif//R64FX_PROCESSOR_MESSAGE_HPP