#include "MachinePoolThread.hpp"
#include "MachinePoolThreadImpl.hpp"
#include "MachineImpl.hpp"

#include <iostream>

#ifndef R64FX_MESSAGE_BUFFER_SIZE
#define R64FX_MESSAGE_BUFFER_SIZE 32
#endif//R64FX_MESSAGE_BUFFER_SIZE

using namespace std;

namespace r64fx{

enum MachineMessageProtocol{
    PickDestination = 0,

    MachineToBeDeployed,
    DeployMachine,
    MachineToBeWithdrawn,
    WithdrawMachine,

    ImplDeployed,
    ImplWithdrawn
};


MachinePoolThread::MachinePoolThread()
{
    m_to_thread   = new CircularBuffer<MachineMessage>(R64FX_MESSAGE_BUFFER_SIZE);
    m_from_thread = new CircularBuffer<MachineMessage>(R64FX_MESSAGE_BUFFER_SIZE);
}


MachinePoolThread::~MachinePoolThread()
{
    delete m_to_thread;
    delete m_from_thread;
}


void MachinePoolThread::start()
{
    struct ThreadStartArgs{
        CircularBuffer<MachineMessage>* to_thread;
        CircularBuffer<MachineMessage>* from_thread;
    };

    auto args = new ThreadStartArgs;
    args->to_thread    = m_to_thread;
    args->from_thread  = m_from_thread;

    run([](void* arg) -> void* {
        auto args = (ThreadStartArgs*) arg;
        MachinePoolThreadImpl impl;
        impl.run(args->to_thread, args->from_thread);
        delete args;
        return nullptr;
    }, args);
}


void MachinePoolThread::join()
{
    Thread::join();
}


void MachinePoolThread::readMessagesFromImpl()
{
    MachineMessage msg;
    while(m_to_thread->read(&msg, 1))
    {
        if(msg.opcode == PickDestination)
        {
            m_dst_iface = (MachineIface*) msg.value;
        }
        else
        {
            if(m_dst_iface)
            {
                if(msg.opcode == ImplDeployed)
                {
                    m_dst_iface->implDeployed((MachineImpl*) msg.value);
                }
            }
            else
            {

            }
        }
    }
}



void MachinePoolThread::deployImpl(MachineImplDeploymentFun fun, MachineIface* iface)
{
    pickDestinationImpl(nullptr);

    MachineMessage msgs[2] = {
        MachineMessage(MachineToBeDeployed, (void*)iface),
        MachineMessage(DeployMachine,       (void*)fun)
    };
    m_to_thread->write(msgs, 2);
}


void MachinePoolThread::withdrawImpl(MachineImplWithdrawalFun fun, MachineImpl* impl)
{
    pickDestinationImpl(nullptr);

    MachineMessage msgs[2] = {
        MachineMessage(MachineToBeWithdrawn, (void*)impl),
        MachineMessage(WithdrawMachine,      (void*)fun)
    };
    m_to_thread->write(msgs, 2);
}


void MachinePoolThread::pickDestinationImpl(MachineImpl* dst)
{
    if(dst != m_dst_impl)
    {
        MachineMessage dstmsg(PickDestination, dst);
        m_to_thread->write(&dstmsg, 1);
        m_dst_impl = dst;
    }
}


void MachinePoolThread::sendMessagesToImpl(MachineImpl* dst, MachineMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(msgs != nullptr);
    assert(nmsgs > 0);
    assert(nmsgs < R64FX_MESSAGE_BUFFER_SIZE);
#endif//R64FX_DEBUG

    pickDestinationImpl(dst);
    m_to_thread->write(msgs, nmsgs);
}

/* ==================================================================================================================== */


void MachinePoolThreadImpl::run(CircularBuffer<MachineMessage>* to_thread, CircularBuffer<MachineMessage>* from_thread)
{
    m_to_thread    = to_thread;
    m_from_thread  = from_thread;
    m_running      = true;

    MachineIface*  machine_to_be_deployed   = nullptr;
    MachineImpl*   machine_to_be_withdrawn  = nullptr;

    while(m_running)
    {
        MachineMessage msg;
        while(m_to_thread->read(&msg, 1))
        {
            if(msg.opcode == PickDestination)
            {
                m_dst_impl = (MachineImpl*) msg.value;
            }
            else
            {
                if(m_dst_impl)
                {
                    m_dst_impl->messageRecievedFromIface(msg);
                }
                else
                {
                    if(msg.opcode == MachineToBeDeployed)
                    {
                        machine_to_be_deployed = (MachineIface*) msg.value;
                    }
                    else if(msg.opcode == DeployMachine)
                    {
#ifdef R64FX_DEBUG
                        assert(machine_to_be_deployed != nullptr);
#endif//R64FX_DEBUG
                        auto deployment_fun = (MachineImplDeploymentFun) msg.value;
                        auto impl = deployment_fun(machine_to_be_deployed, this);
                        MachineMessage response_msg(ImplDeployed, impl);
                    }
                    else if(msg.opcode == MachineToBeWithdrawn)
                    {
                        machine_to_be_withdrawn = (MachineImpl*) msg.value;
                    }
                    else if(msg.opcode == WithdrawMachine)
                    {
#ifdef R64FX_DEBUG
                        assert(machine_to_be_withdrawn != nullptr);
#endif//R64FX_DEBUG
                        auto withdrawal_fun = (MachineImplWithdrawalFun) msg.value;
                        withdrawal_fun(machine_to_be_withdrawn, this);
                    }
#ifdef R64FX_DEBUG
                    else
                    {
                        cerr << "Unknown MachineMessage!\n";
                        abort();
                    }
#endif//R64FX_DEBUG
                }
            }
        }

        /* Do Work Here! */
    }
}


void MachinePoolThreadImpl::pickDestinationIface(MachineIface* dst)
{
    if(dst != m_dst_iface)
    {
        MachineMessage dstmsg(PickDestination, dst);
        m_from_thread->write(&dstmsg, 1);
        m_dst_iface = dst;
    }
}

void MachinePoolThreadImpl::sendMessagesToIface(MachineIface* dst, MachineMessage* msgs, int nmsgs)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(msgs != nullptr);
    assert(nmsgs > 0);
    assert(nmsgs < R64FX_MESSAGE_BUFFER_SIZE);
#endif//R64FX_DEBUG

    pickDestinationIface(dst);
    m_from_thread->write(msgs, nmsgs);
}

}//namespace r64fx
