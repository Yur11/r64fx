#include <iostream>
#include <string>
#include <cmath>
#include "SoundDriver.hpp"
#include "TimeUtils.hpp"
#include "Thread.hpp"
#include "jit.hpp"

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

constexpr unsigned long fact(unsigned long n) { if(n == 0) return 1; return n * fact(n - 1); }


int   g_buffer_size      = 0.0f;
float g_sample_rate      = 0.0f;
float g_sample_rate_rcp  = 0.0f;


struct Data{
    struct{
        float pi_int_rcp  = PI_INT_RCP;
        float one         = 1.0f;
        float f2_rcp      = 1.0f / fact(2);
        float f4_rcp      = 1.0f / fact(4);
    }v0;

    struct{
        float f6_rcp      = 1.0f / fact(6);
        float f8_rcp      = 1.0f / fact(8);
        float f10_rcp     = 1.0f / fact(10);
        float f           = 0.0f;
    }v1;

    struct{
        int osc[4] = {0, 0, 0, 0};
    }v2;

    struct{
        int increment[4] = {0, 0, 0, 0};
    }v3;
};

Data* g_data = nullptr;

void init_data()
{
    auto buff = alloc_pages(1);
    g_data = new(buff) Data;
}

void free_data()
{
    free((void*)g_data);
}


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


inline float osc_cos(Data* data)
{
    int osc = data->v2.osc[0];
    float s = float(osc) * data->v0.pi_int_rcp;

    s = s*s;
    float x = s;
    float val = 1.0f;
            val -= x * data->v0.f2_rcp;
    x *= s; val += x * data->v0.f4_rcp;
    x *= s; val -= x * data->v1.f6_rcp;
    x *= s; val += x * data->v1.f8_rcp;
    x *= s; val -= x * data->v1.f10_rcp;

    osc += g_data->v3.increment[0];
    g_data->v2.osc[0] = osc;
    return val;
}


class SynthAssembler : public Assembler{
public:
    SynthAssembler() { resize(0, 1); }

    auto genOscCos()
    {
        auto fun_addr = codeEnd();
        auto s = xmm0;
        auto x = xmm1;
        auto f = xmm2;
        auto c = xmm3;
        auto v = xmm4;

        MOVAPS   (c, Base(rdi));

        /* Osc Clock */
        MOVAPS   (f, Base(rdi) + Disp(sizeof(float) * 8));
        CVTDQ2PS (s, f); //Use osc value before incrementing.
        PADDD    (f, Base(rdi) + Disp(sizeof(float) * 12));
        MOVAPS   (Base(rdi) + Disp(sizeof(float) * 8), f);

        PSHUFD   (f, c, Shuf(0, 0, 0, 0));
        MULPS    (s, f);


        /* Cosine Wave */
        MULPS  (s, s);
        MOVAPS (x, s);

        // 1
        PSHUFD (v, c, Shuf(1, 1, 1, 1));

        // X^2
        PSHUFD (f, c, Shuf(2, 2, 2, 2));
        MULPS  (f, x);
        SUBPS  (v, f);

        // X^4
        MULPS  (x, s);
        PSHUFD (f, c, Shuf(3, 3, 3, 3));
        MULPS  (f, x);
        ADDPS  (v, f);

        MOVAPS (c, Base(rdi) + Disp(sizeof(float) * 4));

        // X^6
        MULPS  (x, s);
        PSHUFD (f, c, Shuf(0, 0, 0, 0));
        MULPS  (f, x);
        SUBPS  (v, f);

        //X^8
        MULPS  (x, s);
        PSHUFD (f, c, Shuf(1, 1, 1, 1));
        MULPS  (f, x);
        ADDPS  (v, f);

        // X^10
        MULPS  (x, s);
        PSHUFD (f, c, Shuf(2, 2, 2, 2));
        MULPS  (f, x);
        SUBPS  (v, f);

        MOVAPS(xmm0, v);

        RET();
        return (Fun<float, Data*>::T*)fun_addr;
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
        g_data->v3.increment[i] = increment;

    while(g_running)
    {
        if(g_port_group->sync())
        {
            for(int i=0; i<g_buffer_size; i++)
            {
//                 float val = osc_cos(g_data) * 0.5f;
                float val = jit_osc_cos(g_data) * 0.5f;
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
    init_data();

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
    free_data();
    return 0;
}
