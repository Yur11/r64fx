#ifndef R64FX_DSP_PROCESSOR_H
#define R64FX_DSP_PROCESSOR_H

#include "jit/jit.h"
#include "shared_sources/LinkedItem.h"
#include <vector>

namespace r64fx{
    
class Slot;
class OutputPort;
class Port;
class InputPort;
class Connection;
    
/** @brief Base class for vector dsp procesors.
 
    These don't actually do any processing themselves 
    but produce machiene code snippets that do the processing.
    
    Each processor has a property called size which determines
    the number of slots that can be allocated.
    
    
 */
class Processor{
    friend class Connection;
    
protected:
    unsigned int block_size = 0;
    unsigned int block_count = 0;
    
public:
    inline unsigned int blockSize() const { return block_size; }
    inline unsigned int blockCount() const { return block_count; }
    inline unsigned int size() const { return blockCount() * blockSize(); }

    Slot* getSlot();
    
    void freeSlot(Slot* slot);
    
#ifdef DEBUG
    void debugSlots();
#endif//DEBUG
    
    /** @brief Render processing code. */
    virtual void render(Assembler &as) = 0;
    
    /** @brief Render code that does the signal routing between processors. */
    void renderRouting(Assembler &as);
    
private:
    /* Slots are kept as a sorted linked list. */
    Slot* first_slot = nullptr;
    
    std::vector<Connection*> outgoing_connections;
    
protected:
    void myPort(Port* port);
    
};


struct Port{
    /** @brief Processor instance that this port belongs to. */
    Processor* processor = nullptr;
    
    /** @brief Port buffer. */
    float* addr = nullptr;

    /** @brief Get address for the specific slot. */
    float* at(Slot* slot);
};


struct OutputPort : public Port{};

struct InputPort : public Port{};


class Slot : public LinkedItem<Slot>{
    friend class Processor;
    int _index;
    Slot(int index) : _index(index) {}
    
    ~Slot()
    {
        remove();
    }
    
public:
    inline int index() const { return _index; }        
};


class Connection{
    friend Connection* make_connection(OutputPort* output_port, Slot* output_slot, InputPort* input_port, Slot* input_slot);
    
    OutputPort* output_port;
    Slot* output_slot;
    InputPort* input_port;
    Slot* input_slot;

public:
    Connection(OutputPort* output_port, Slot* output_slot, InputPort* input_port, Slot* input_slot);
    
    ~Connection();
    
    /** @brief Memory location to read data from. */
    float* source() { return output_port->at(output_slot); }
    
    /** @brief Memory location to write data to. */
    float* sink() { return input_port->at(input_slot); }
};


Connection* make_connection(OutputPort* output_port, Slot* output_slot, InputPort* input_port, Slot* input_slot);

}//namespace r64fx

#endif//R64FX_DSP_PROCESSOR_H