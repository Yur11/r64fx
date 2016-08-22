#include "PlayerController.hpp"
#include "PlayerView.hpp"
#include "PlayerMachine.hpp"
#include "SoundFilePool.hpp"
#include "RouterMachine.hpp"

#include "Timer.hpp"
#include "sleep.hpp"
#include "SoundDriverMachine.hpp"
#include "MachinePool.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
SoundFilePool* g_sound_file_pool = nullptr;
    
class PlayerControllerPrivate
: public PlayerViewEventIface
{
    PlayerView* m_view = nullptr;
    PlayerMachine* m_machine = nullptr;
    
    MachinePool* m_pool = nullptr;
    bool m_running = true;    
    SoundDriverMachine* m_sound_driver_machine = nullptr;
    MachineSignalSink* m_master_out = nullptr;
    
    SoundFileDataPtr m_sound_data;
    
public:
    PlayerControllerPrivate()
    {
        m_view = new PlayerView(this);
        m_view->show();
        
        m_pool = new MachinePool;
        
        m_machine = new PlayerMachine(m_pool);
        m_machine->deploy();
        
        m_sound_driver_machine = new SoundDriverMachine(m_pool);
        m_sound_driver_machine->deploy();
        m_sound_driver_machine->enable();
        m_master_out = m_sound_driver_machine->createAudioOutput("out", 2);
                
        while(!m_machine->isReady() && !m_master_out->impl())
        {
            cout << "Waiting for impl!\n";
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        cout << "Done!\n";

        RouterMachine::singletonInstance(m_pool)->makeConnection(m_machine->output(), m_master_out);
        
        m_sound_driver_machine->connect("r64fx:out_1", "system:playback_1");
        m_sound_driver_machine->connect("r64fx:out_2", "system:playback_2");
        
        if(!g_sound_file_pool)
        {
            g_sound_file_pool = new SoundFilePool;
        }
    }
    
    virtual ~PlayerControllerPrivate()
    {
        m_view->hide();
        delete m_view;
        
        m_machine->withdraw();
        delete m_machine;
        
        m_sound_driver_machine->withdraw();
        delete m_sound_driver_machine;
        
        delete m_pool;
        
        if(g_sound_file_pool)
        {
            delete g_sound_file_pool;
            g_sound_file_pool = nullptr;
        }
    }
    
    virtual int frameCount()
    {
        if(m_sound_data)
        {
            return m_sound_data.frameCount();
        }
        return 0;
    }

    virtual int componentCount()
    {
        if(m_sound_data)
        {
            return m_sound_data.componentCount();
        }
        return 0;
    }

    virtual bool loadAudioFile(const std::string &path)
    {
        m_sound_data = g_sound_file_pool->load(path);
        if(m_sound_data)
        {
            m_machine->replaceSample(m_sound_data);
            m_view->notifyLoad(true);
            return true;
        }
        return false;
    }
    
    virtual void unloadCurrentFile()
    {
        
    }

    virtual void loadWaveform(int begin_idx, int end_idx, int component, int pixel_count, float* out)
    {
        int range = end_idx - begin_idx;
        int frames_per_pixel = range / pixel_count;
        for(int p=0; p<pixel_count; p++)
        {
            float min_value = 0;
            float max_value = 0;
            for(int f=0; f<frames_per_pixel; f++)
            {
                int ff = p * frames_per_pixel + f;
                float value = m_sound_data.data()->frame(ff)[component];
                if(value > max_value)
                    max_value = value;
                if(value < min_value)
                    min_value = value;
            }
            out[p*2] = min_value;
            out[p*2 + 1] = max_value;
        }
    }

    virtual bool hasData()
    {
        return false;
    }
    
    virtual void close()
    {
        cout << "close!\n";
    }
    
    void exec()
    {
        while(m_running)
        {
            Timer::runTimers();
            sleep_microseconds(1000);
        }
    }
};
    
    
PlayerController::PlayerController()
{
    m = new PlayerControllerPrivate;
}
    
    
PlayerController::~PlayerController()
{
    delete m;
}


void PlayerController::exec()
{
    m->exec();
}
    
}//namespace r64fx