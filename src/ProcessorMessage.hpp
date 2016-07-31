#ifndef R64FX_PROCESSOR_MESSAGE_HPP
#define R64FX_PROCESSOR_MESSAGE_HPP

namespace r64fx{

struct ProcessorMessage{
    unsigned long header = 0;
    unsigned long value = 0;

    ProcessorMessage(unsigned long header, unsigned long value)
    : header(header)
    , value(value)
    {}

    ProcessorMessage() {}
};

}//namespace r64fx

#endif//R64FX_PROCESSOR_MESSAGE_HPP