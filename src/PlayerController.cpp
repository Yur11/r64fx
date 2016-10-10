#include "PlayerController.hpp"
#include "PlayerView.hpp"
#include "PlayerMachine.hpp"
#include "SoundFilePool.hpp"
#include "RouterMachine.hpp"
#include "FilterMachine.hpp"

#include "Timer.hpp"
#include "sleep.hpp"
#include "SoundDriverMachine.hpp"
#include "MachinePool.hpp"

#include "FilterView.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

SoundFilePool* g_sound_file_pool = nullptr;

class PlayerControllerPrivate
: public PlayerViewControllerIface
, public FilterViewControllerIface
{
    PlayerView* m_view = nullptr;
    PlayerMachine* m_machine = nullptr;

    FilterMachine* m_filter_machine = nullptr;

    MachinePool* m_pool = nullptr;
    bool m_running = true;
    SoundDriverMachine* m_sound_driver_machine = nullptr;
    MachineSignalSink* m_master_out = nullptr;

    SoundFileDataPtr m_sound_data;

    FilterView* m_filter_view = nullptr;

public:
    PlayerControllerPrivate()
    {
        m_view = new PlayerView(this);
//         m_view->show();

        m_pool = new MachinePool;

        m_machine = new PlayerMachine(m_pool);
        m_machine->deploy();
        while(!m_machine->isReady())
        {
            cout << "Waiting for machine!\n";
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        cout << "Machine done!\n";

        m_filter_machine = new FilterMachine(m_pool);
        m_filter_machine->deploy();
        while(!m_filter_machine->isReady())
        {
            cout << "Waiting for filter deployment!\n";
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        cout << "Fiter deployment done!\n";

        cout << m_filter_machine->sink()->impl() << ", " << m_filter_machine->source()->impl() << "\n";
        m_filter_machine->setSize(2);
        while(!m_filter_machine->isReady())
        {
            cout << "Waiting for filter size!\n";
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        cout << "Filter size done!\n";

        m_sound_driver_machine = new SoundDriverMachine(m_pool);
        m_sound_driver_machine->deploy();
        m_sound_driver_machine->enable();
        m_master_out = m_sound_driver_machine->createAudioOutput("out", 2);
        while(!m_master_out->impl())
        {
            cout << "Waiting for m_master_out!\n";
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        cout << "m_master_out done!\n";
        cout << "master_out: " << m_master_out << " -> " << m_master_out->impl() << "\n";

        RouterMachine::singletonInstance(m_pool)->makeConnection(m_machine->output(), m_filter_machine->sink());
        RouterMachine::singletonInstance(m_pool)->makeConnection(m_filter_machine->source(), m_master_out);

        m_sound_driver_machine->connect("r64fx:out_1", "system:playback_1");
        m_sound_driver_machine->connect("r64fx:out_2", "system:playback_2");

        if(!g_sound_file_pool)
        {
            g_sound_file_pool = new SoundFilePool;
        }

        m_filter_machine->setPole(0.01f);
        m_filter_view = new FilterView(this);
//         m_filter_view->show();
    }

    virtual ~PlayerControllerPrivate()
    {
        m_view->hide();
        delete m_view;

        m_machine->withdraw();
        delete m_machine;

        m_filter_machine->withdraw();
        delete m_filter_machine;

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

    virtual float sampleRate()
    {
        if(m_sound_data)
        {
            return m_sound_data.sampleRate();
        }
        return 0.0f;
    }

    virtual bool loadAudioFile(const std::string &path)
    {
        m_sound_data = g_sound_file_pool->load(path);
        if(m_sound_data)
        {
            m_machine->replaceSample(m_sound_data);
            return true;
        }
        return false;
    }

    virtual bool hasFile()
    {
        return m_sound_data;
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

    virtual void changePitch(float pitch)
    {
        m_machine->setPitch(pitch);
    }

    virtual void changeGain(float pitch)
    {
        m_machine->setGain(pitch);
    }

    virtual void setPlayheadTime(float seconds)
    {
        m_machine->setPlayheadTime(seconds);
    }

    virtual float playheadTime()
    {
        return m_machine->playheadTime();
    }

    virtual void play()
    {
        m_machine->play();
    }

    virtual void stop()
    {
        m_machine->stop();
    }

    virtual bool isPlaying()
    {
        return m_machine->isPlaying();
    }

    virtual bool hasData()
    {
        return false;
    }

    virtual void close()
    {
        cout << "Close!\n";
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

}//namespace r64fx
