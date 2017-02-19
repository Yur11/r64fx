#include "FilterMachine.hpp"
#include "MachineImpl.hpp"
#include "MachineFlags.hpp"
#include "MachinePortImpl.hpp"
#include "MachinePoolThreadImpl.hpp"
#include "SignalNode_Filter.hpp"


#include <iostream>
using namespace std;


namespace r64fx{

namespace{
    enum{
        SetSize = 1,
        SetPole,

        SizeSet,
        SinkCreated,
        SourceCreated,
    };
}//namespace


struct FilterMachineImpl : public MachineImpl{
    SignalNode_Filter* m_filter = 0;
    MachineSinkImpl*    m_sink_impl    = nullptr;
    MachineSourceImpl*  m_source_impl  = nullptr;

    FilterMachineImpl(MachineIface* iface)
    : MachineImpl(iface)
    {
//         m_filter = new SignalNode_Filter;
//         ctx()->main_subgraph->addItem(m_filter);
//
//         m_sink_impl    = new MachineSinkImpl;
//         sendMessage(SinkCreated, (unsigned long) m_sink_impl);
//
//         m_source_impl  = new MachineSourceImpl;
//         sendMessage(SourceCreated, (unsigned long) m_source_impl);
//
//         cout << "Filter Deployed!\n";
    }

    virtual ~FilterMachineImpl()
    {
//         setSize(0);
//
//         if(m_filter)
//             delete m_filter;
//
//         if(m_sink_impl)
//             delete m_sink_impl;
//
//         if(m_source_impl)
//             delete m_source_impl;
//
//         cout << "Filter Withdrawn!\n";
    }

    virtual void messageRecievedFromIface(const MachineMessage &msg)
    {

    }

//     virtual void messageRecieved(const MachineMessage &msg)
//     {
// //         if(msg.opcode == SetSize)
// //         {
// //             setSize((int)msg.value);
// //         }
// //         else if(msg.opcode == SetPole)
// //         {
// //             m_filter->setPole(MsgVal(msg.value).f(0));
// //         }
//     }
//
//     void setSize(int size)
//     {
// //         m_filter->setSize(size);
// //
// //         m_sink_impl->sinks.resize(size);
// //         m_source_impl->sources.resize(size);
// //         for(int i=0; i<size; i++)
// //         {
// //             m_sink_impl->sinks.at(i) = m_filter->sink(i);
// //             m_source_impl->sources.at(i) = m_filter->source(i);
// //         }
// //
// //         sendMessage(SizeSet, (unsigned long)size);
//     }
//
//     virtual void cycleStarted()
//     {
//
//     }
//
//     virtual void cycleEnded()
//     {
//
//     }
};


namespace{

MachineImpl* deploy_impl(MachineIface* iface, MachinePoolThreadImpl*)
{
    return new FilterMachineImpl(iface);
}


void withdraw_impl(MachineImpl* impl)
{
    auto machine_impl = static_cast<FilterMachineImpl*>(impl);
    delete machine_impl;
}

}

/* ================================================================================================================== */


FilterMachine::FilterMachine(MachinePool* pool)
: MachineIface(pool, deploy_impl, withdraw_impl)
, m_sink(this, "in")
, m_source(this, "out")
{

}


FilterMachine::~FilterMachine()
{
//     m_flags &= ~R64FX_MACHINE_IS_READY;
}


void FilterMachine::setSize(int size)
{
//     m_flags &= ~R64FX_MACHINE_IS_READY;
//
//     packMessage(SetSize, (unsigned long)size);
// //     RouterMachine::singletonInstance(pool())->packConnectionUpdatesFor(&m_sink);
// //     RouterMachine::singletonInstance(pool())->packConnectionUpdatesFor(&m_source);
//     sendPack();
}


int FilterMachine::size() const
{
    return m_size;
}


void FilterMachine::setPole(float pole)
{
//     sendMessage(SetPole, MsgVal(pole));
}


MachineSignalSink* FilterMachine::sink()
{
    return &m_sink;
}


MachineSignalSource* FilterMachine::source()
{
    return &m_source;
}


void FilterMachine::forEachPort(void (*fun)(MachinePort* port, MachineIface* machine, void* arg), void* arg)
{
    fun(&m_sink,   this, arg);
    fun(&m_source, this, arg);
}


// void FilterMachine::dispatchMessage(const MachineMessage &msg)
// {
//     if(msg.opcode == SizeSet)
//     {
//         m_flags |= R64FX_MACHINE_IS_READY;
//         m_size = int(msg.value);
//         cout << "SizeSet: " << m_size << "\n";
//     }
//     else if(msg.opcode == SinkCreated)
//     {
//         cout << "SinkCreated!\n";
//         auto impl = (MachineSinkImpl*) msg.value;
//         m_sink.setImpl(impl);
//     }
//     else if(msg.opcode == SourceCreated)
//     {
//         cout << "SourceCreated!\n";
//         m_flags |= R64FX_MACHINE_IS_READY;
//         auto impl = (MachineSourceImpl*) msg.value;
//         m_source.setImpl(impl);
//     }
// }

}//namespace r64fx
