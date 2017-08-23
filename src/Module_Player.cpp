#include "Module_Player.hpp"
#include "ModulePrivate.hpp"
#include "SoundDriver.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

struct PlayerDeploymentArgs{
    SoundDriverAudioInput*  audio_input_port    = nullptr;
    SoundDriverAudioOutput* audio_output_port   = nullptr;
};

struct PlayerWithdrawalArgs{
    SoundDriverAudioInput*   audio_input_port   = nullptr;
    SoundDriverAudioOutput*  audio_output_port  = nullptr;
};

R64FX_DECL_MODULE_AGENTS(Player)


/*======= Worker Thread =======*/

class PlayerThreadObjectImpl : public ModuleThreadObjectImpl{
    SoundDriverAudioInput* m_audio_input_port  = nullptr;
    SoundDriverAudioOutput* m_audio_output_port = nullptr;
    SoundDriverMidiInput*   m_midi_input_port   = nullptr;

public:
    PlayerThreadObjectImpl(PlayerDeploymentAgent* agent, R64FX_DEF_THREAD_OBJECT_IMPL_ARGS)
    : ModuleThreadObjectImpl(agent, R64FX_THREAD_OBJECT_IMPL_ARGS)
    {
        m_audio_input_port   = agent->audio_input_port;
        m_audio_output_port  = agent->audio_output_port;

        setPrologue([](void* arg){
            auto self = (PlayerThreadObjectImpl*) arg;
            self->prologue();
        }, this);

        setEpilogue([](void* arg){
            auto self = (PlayerThreadObjectImpl*) arg;
            self->epilogue();
        }, this);
    }

    ~PlayerThreadObjectImpl()
    {
    }

    void storeWithdrawalArgs(PlayerWithdrawalAgent* agent)
    {
        agent->audio_input_port   = m_audio_input_port;
        agent->audio_output_port  = m_audio_output_port;
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
            default:
            {
                std::cerr << "Bad Message from iface!\n";
                break;
            }
        }
    }

    inline void prologue()
    {

    }

    inline void epilogue()
    {

    }
};

R64FX_DEF_MODULE_AGENTS(Player)



/*======= Main Thread =======*/

class PlayerThreadObjectIface : public ModuleThreadObjectIface{

public:
    ModuleCallback  done      = nullptr;
    void*           done_arg  = nullptr;

private:
    virtual ModuleDeploymentAgent* newModuleDeploymentAgent()  override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG

        auto filter_agent = new PlayerDeploymentAgent;

        filter_agent->audio_input_port = sd->newAudioInput("filter_in");
#ifdef R64FX_DEBUG
        assert(filter_agent->audio_input_port != nullptr);
#endif//R64FX_DEBUG

        filter_agent->audio_output_port = sd->newAudioOutput("filter_out");
#ifdef R64FX_DEBUG
        assert(filter_agent->audio_output_port != nullptr);
#endif//R64FX_DEBUG
        sd->connect("r64fx:filter_out", "system:playback_1");
        sd->connect("r64fx:filter_out", "system:playback_2");
        return filter_agent;
    }

    virtual void deleteModuleDeploymentAgent(ModuleDeploymentAgent* agent) override final
    {
        delete agent;
    }

    virtual ModuleWithdrawalAgent* newModuleWithdrawalAgent() override final
    {
        return new PlayerWithdrawalAgent;
    }

    virtual void deleteModuleWithdrawalAgent(ModuleWithdrawalAgent* agent) override final
    {
        auto sd = soundDriver();
#ifdef R64FX_DEBUG
        assert(sd != nullptr);
#endif//R64FX_DEBUG

        auto filter_agent = static_cast<PlayerWithdrawalAgent*>(agent);
        sd->deletePort(filter_agent->audio_output_port);

        delete filter_agent;
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) override final
    {
        switch(msg.key())
        {
            default:
            {
                std::cerr << "Bad message from impl!\n";
                break;
            }
        }
    }
};

#define m_thread_object_iface ((PlayerThreadObjectIface*)m)


Module_Player::Module_Player()
{
    
}


Module_Player::~Module_Player()
{
    
}


bool Module_Player::engage(ModuleCallback done, void* done_arg)
{
    if(!m)
        m = new(std::nothrow) PlayerThreadObjectIface;
    if(!m)
        return false;
#ifdef R64FX_DEBUG
    assert(!isEngaged());
    assert(!engagementPending());
#endif//R64FX_DEBUG

    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        auto filter_iface = static_cast<PlayerThreadObjectIface*>(iface);
        if(filter_iface->done)
        {
            filter_iface->done((Module*)arg, filter_iface->done_arg);
        }
    }, this);
    return true;
}


void Module_Player::disengage(ModuleCallback done, void* done_arg)
{
    m_thread_object_iface->done = done;
    m_thread_object_iface->done_arg = done_arg;
    m_thread_object_iface->withdraw([](ThreadObjectIface* iface, void* arg){
        auto filter_iface = static_cast<PlayerThreadObjectIface*>(iface);
        if(filter_iface->done)
        {
            filter_iface->done((Module*)arg, filter_iface->done_arg);
        }
    }, this);
}


bool Module_Player::isEngaged()
{
    return m != nullptr && m_thread_object_iface->isDeployed() && (!m_thread_object_iface->isPending());
}


bool Module_Player::engagementPending()
{
    return m_thread_object_iface->isPending();
}

}//namespace r64fx
