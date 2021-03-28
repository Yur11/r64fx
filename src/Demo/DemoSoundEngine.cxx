#include <iostream>

#include "../MainLoop.hpp"
#include "../SoundDriver.hpp"
#include "../Thread.hpp"
#include "../TimeUtils.hpp"

using namespace std;

namespace r64fx{

struct Impl{
    bool running = false;
    SoundDriverPortGroup* pg = nullptr;
    SoundDriverAudioInput *left_in = nullptr, *right_in = nullptr;
    SoundDriverAudioOutput *left_out = nullptr, *right_out = nullptr;
};

void run(Impl* m)
{
    float buff[256];

    for(int i=0; i<256; i++)
    {
        buff[i] = (i<128 ? +1.0f : -1.0f) * 0.3f;
    }

    m->running = true;
    m->pg->enable();
    while(m->running)
    {
        if(m->pg->sync())
        {
            m->pg->updatePort(m->left_out, buff);
            m->pg->updatePort(m->right_out, buff);
            m->pg->done();
        }
        else
        {
            sleep_nanoseconds(10000);
        }
    }
    m->pg->disable();
}


class DemoSoundEngine : public MainLoop{
    SoundDriver* m_sound_driver;
    Thread m_thread;
    Impl* m = nullptr;

public:
    DemoSoundEngine()
    {
        m_sound_driver = SoundDriver::newInstance(SoundDriver::Type::Jack, "r64fx");
        m = new Impl;
    }

    ~DemoSoundEngine()
    {
        if(m_sound_driver)
            SoundDriver::deleteInstance(m_sound_driver);

        if(m)
            delete m;
    }

    int run()
    {
        m_sound_driver->enable();

        m->pg = m_sound_driver->newPortGroup();
        m->left_in = m_sound_driver->newAudioInput("left_in");
        m->right_in = m_sound_driver->newAudioInput("right_in");
        m->left_out = m_sound_driver->newAudioOutput("left_out");
        m->right_out = m_sound_driver->newAudioOutput("right_out");
        m_sound_driver->sync();

        m_thread.run([](void* arg) -> void* { r64fx::run((Impl*)arg); return nullptr; }, m);
        MainLoop::run();

        m->running = false;
        m_thread.join();

        m_sound_driver->deletePort(m->left_in);
        m_sound_driver->deletePort(m->right_in);
        m_sound_driver->deletePort(m->left_out);
        m_sound_driver->deletePort(m->right_out);
        m_sound_driver->deletePortGroup(m->pg);
        m_sound_driver->sync();
        m_sound_driver->disable();

        return 0;
    }
};

}//namespace r64fx