#include "Processor.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
#endif//DEBUG

namespace r64fx{
    
Slot* Processor::getSlot()
{
#ifdef DEBUG
    assert(block_size > 0);
    assert(block_count > 0);
#endif//DEBUG
    
    /* No nodes. */
    if(first_slot == nullptr)
    {
        first_slot = new Slot(0);
        return first_slot;
    }

    /* First slot has been removed. */
    if(first_slot->index() > 0)
    {
        auto slot = new Slot(0);
        slot->insert(first_slot);
        first_slot = slot;
        return slot;
    }
    
    auto slot = first_slot;
    for(;;)
    {
        if(slot->index() == (int)size() - 1)
            return nullptr;
        
        if(slot->nextItem() == nullptr || slot->nextItem()->index() - slot->index() > 1)
        {
            slot->insert(new Slot(slot->index()+1));
            return slot->nextItem();
        }
        else 
        {
            slot = slot->nextItem();
        }
    }    
}


void Processor::freeSlot(Slot* slot)
{
    if(slot == first_slot)
        first_slot = slot->nextItem();
    
    slot->remove();
}


#ifdef DEBUG
void Processor::debugSlots()
{
    for(auto slot=first_slot; slot!=nullptr; slot=slot->nextItem())
    {
        std::cout << slot->index() << "\n";
    }
}
#endif//DEBUG


Connection::Connection(OutputPort* output_port, Slot* output_slot, InputPort* input_port, Slot* input_slot)
: output_port(output_port)
, output_slot(output_slot)
, input_port(input_port)
, input_slot(input_slot)
{
    output_port->processor->outgoing_connections.push_back(this);
}


Connection::~Connection()
{
    auto &outgoing_connections = output_port->processor->outgoing_connections;
    auto it = outgoing_connections.begin();
    while(it != outgoing_connections.end() || *it != this) it++;
#ifdef DEBUG
    assert(it != outgoing_connections.end());
#endif//DEBUG
    outgoing_connections.erase(it);
}


Connection* make_connection(OutputPort* output_port, Slot* output_slot, InputPort* input_port, Slot* input_slot)
{
    return new Connection(output_port, output_slot, input_port, input_slot);
}


void Processor::renderRouting(Assembler &as)
{
#ifdef DEBUG
    if(outgoing_connections.empty())
        as.dump << "---No routing---\n";
    else
        as.dump << "---Routing---\n";
#endif//DEBUG
    float* prev_source = nullptr;
    for(auto c : outgoing_connections)
    {
        auto sink = c->sink();
        auto source = c->source();
        if(source != prev_source)
        {
            as.mov(eax, Mem32(source));
            prev_source = source;
        }
        as.mov(Mem32(sink), eax);
    }
}


void Processor::myPort(Port* port)
{
    port->processor = this;
}


float* Port::at(Slot* slot)
{
    return addr + slot->index();
}
    
}//namespace r64fx