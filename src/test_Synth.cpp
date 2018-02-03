#include <iostream>
#include <string>
#include <cmath>
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "Thread.hpp"
#include "jit_algorithms.hpp"

#define CLIENT_NAME "test_Synth"
#define SOUND_DRIVER_TYPE SoundDriver::Type::Jack

using namespace std;
using namespace r64fx;

namespace{

bool g_running = true;

SoundDriverPortGroup* g_port_group = nullptr;
SoundDriverAudioOutput *g_audio_output = nullptr;

constexpr float  INT_RCP     = 1.0f / int(0x7FFFFFFF);
constexpr float  PI_INT_RCP  = INT_RCP * 3.1415926535897932384626f;


int   g_buffer_size      = 0.0f;
float g_sample_rate      = 0.0f;
float g_sample_rate_rcp  = 0.0f;

JitAlgorithmConstants g_contants __attribute__((aligned(64)));

SineOscBuffers g_buffers __attribute__((aligned(64)));


template<typename ReturnT, typename... ArgT> struct Fun{
    typedef ReturnT (T)(ArgT...);
};


inline void wait_sync()
{
    while(!g_port_group->sync())
        sleep_nanoseconds(1000 * 1000);
}

inline void done()
{
    g_port_group->done();
}

inline void one_cycle()
{
    wait_sync();
    done();
}


inline float osc_cos(JitAlgorithmConstants* constants, SineOscBuffers* buffers)
{
    int osc = buffers->v0.osc[0];
    float s = float(osc) * constants->v0.pi_int_rcp;

    s = s*s;
    float x = s;
    float val = 1.0f;
            val -= x * constants->v0.f2_rcp;
    x *= s; val += x * constants->v0.f4_rcp;
    x *= s; val -= x * constants->v1.f6_rcp;
    x *= s; val += x * constants->v1.f8_rcp;
    x *= s; val -= x * constants->v1.f10_rcp;

    osc += buffers->v1.increment[0];
    buffers->v0.osc[0] = osc;
    return val;
}


class SynthAssembler : public Assembler{
public:
    SynthAssembler() { resize(0, 1); }

    auto genOscCos()
    {
        auto fun_addr = codeEnd();
        gen_sine_osc(this, rdi, rsi, xmm0, xmm1, xmm2, xmm3, xmm4);
        RET();
        return (Fun<float, JitAlgorithmConstants*, SineOscBuffers*>::T*)fun_addr;
    }

    auto genFun()
    {
        auto fun_addr = codeEnd();
        MOV(rax, rsi);
        RET();
        return (Fun<long, long, long>::T*)fun_addr;
    }
};


void* worker_thread(void* arg)
{
    SynthAssembler sa;
    auto jit_osc_cos = sa.genOscCos();

    auto sd = SoundDriver::newInstance(SOUND_DRIVER_TYPE, CLIENT_NAME);
    sd->enable();

    g_port_group = sd->newPortGroup();
    g_port_group->enable();
    g_audio_output = sd->newAudioOutput("out");

    sd->connect(CLIENT_NAME":out", "system:playback_1");
    sd->connect(CLIENT_NAME":out", "system:playback_2");

    g_buffer_size = sd->bufferSize();
    float* audio_buffer = new float[g_buffer_size];

    wait_sync();
    g_port_group->updatePort(g_audio_output, audio_buffer);
    done();

    g_sample_rate = sd->sampleRate();
    g_sample_rate_rcp = 1.0f / g_sample_rate;

    float freq = 440.0f;
    int increment = freq * g_sample_rate_rcp * 0xFFFFFFFFU;
    for(int i=0; i<4; i++)
        g_buffers.v1.increment[i] = increment;

    while(g_running)
    {
        if(g_port_group->sync())
        {
            for(int i=0; i<g_buffer_size; i++)
            {
//                 float val = osc_cos(&g_contants, &g_buffers) * 0.5f;
                float val = jit_osc_cos(&g_contants, &g_buffers) * 0.5f;
                audio_buffer[i] = val;
            }
            g_port_group->done();
        }
        else
        {
            sleep_nanoseconds(1000 * 1000);
        }
    }

    wait_sync();
    g_port_group->updatePort(g_audio_output, nullptr);
    done();
    one_cycle();
    sd->deletePort(g_audio_output);
    g_port_group->disable();
    one_cycle();
    sd->deletePortGroup(g_port_group);

    while(!sd->isClean())
    {
        sd->sync();
        sleep_nanoseconds(300 * 1000 * 1000);
    }

    sd->disable();
    SoundDriver::deleteInstance(sd);
    return nullptr;
}

}//namespace


int main()
{
    Thread wt;
    wt.run(worker_thread);

    while(g_running)
    {
        string str;
        cin >> str;
        if(str == "q" || str == "quit" || str == "exit" || str == "die")
        {
            g_running = false;
        }
        else
        {
            sleep_nanoseconds(300 * 1000 * 1000);
        }
    }

    wt.join();
    return 0;
}
