#include <iostream>
#include <iomanip>
#include "jit.h"

using namespace std;
using namespace r64fx;

typedef long int (*Fun)(void);

template<typename T>
void dump(void* pp)
{
    auto p = (T*) pp;
    for(int i=0; i<3; i++)
        cout << p[i] << ", ";
    cout << p[3] << "\n";
}


float zero[4] = { 0.0, 0.0, 0.0, 0.0 };
float one[4] = { 1.0, 1.0, 1.0, 1.0 };

float sr_value = 48000;
float samplerate[4] = { sr_value, sr_value, sr_value, sr_value };
float samplerate_rcp[4] = { sr_value/1.0f, sr_value/1.0f, sr_value/1.0f, sr_value/1.0f };

float ref_value[4] =  { 0.5, 0.5, 0.5, 0.5 };
float trig_value[4] = { 1.0, 1.0, 1.0, 1.0 };

float attack_step[4]  = { 0.1, 0.1, 0.1, 0.1 };
float decay_step[4]   = { -0.2, -0.2, -0.2, -0.2 };
float sustain_value[4] = { 0.5, 0.5, 0.5, 0.5 };
float release_step[4] = { 0.3, 0.3, 0.3, 0.3 };

float value[4] = { 0.0, 0.0, 0.0, 0.0 };

static const int AttackState = 1;
static const int DecayState = 2;
static const int ReleaseState = 3;

int state[4] = {
    AttackState,
    AttackState,
    DecayState,
    ReleaseState
};


int attack_state[4] = {
    AttackState,
    AttackState,
    AttackState,
    AttackState
};


int decay_state[4] = {
    DecayState,
    DecayState,
    DecayState,
    DecayState
};


int release_state[4] = {
    ReleaseState,
    ReleaseState,
    ReleaseState,
    ReleaseState
};


const int buff_size = 100;
int buff[buff_size];

int d[4];


int main()
{
    auto r1 = xmm0;
    auto r2 = xmm1;
    auto r3 = xmm2;
        
    CodeBuffer cb;
    Assembler as(cb);

    as.mov(rbx, Imm64(&buff));
    as.mov(rcx, buff_size);
    auto mark = Mem8(as.ip());
    
    /* sum = 0 */
    as.xorps(r2, r2);

    /* Attack */
    as.movaps(r1, state);
    as.cmpeqps(r1, attack_state);
    as.andps(r1, attack_step);
    as.orps(r2, r1);
    
    /* Decay */
    as.movaps(r1, state);
    as.cmpeqps(r1, decay_state);
    as.andps(r1, decay_step);
    as.orps(r2, r1);
    
    /* Release */
    as.movaps(r1, state);
    as.cmpeqps(r1, release_state);
    as.andps(r1, release_step);
    as.orps(r2, r1);
    
    /* sum += .. */
    as.addps(r2, value);
    
    /* Lower boundary */
    as.movaps(r1, state);
    as.cmpeqps(r1, decay_state);
    as.movaps(r3, r1);
    as.andps(r1, sustain_value);
    as.xorps(r3, r3);
    as.andps(r3, zero);
    as.orps(r1, r3);
    as.maxps(r1, r2);
    as.movaps(r2, r1);
    
    /* Upper boundary */
    as.minps(r1, one);
    
    /* Resulting value */
    as.movaps(value, r1);
    
    /* State change */
//     as.cmpnltps(r2, one);
//     as.movaps(r1, r2);
//     as.andps(r1, attack_state);
//     as.xorps(r2, r2);
//     as.andps(r2, decay_state);
//     as.orps(r1, r2);
//     as.movaps(r2, ref_value);
//     as.cmpltps(r2, trig_value);
//     as.movaps(r3, r2);
//     as.xorps(r3, r3);
//     as.andps(r1, r3);
//     as.andps(r2, release_state);
//     as.orps(r1, r2);
//     as.movaps(state, r2);
        
    as.movaps(d, r1);
    
    as.mov(eax, Mem32(state));
    as.mov(Base(rbx), eax);
    as.add(rbx, 4);
    as.sub(rcx, 1);
    as.jnz(mark);
    
    as.ret();
    
    Fun fun = (Fun) as.getFun();
    fun(); 

    for(int i=0; i<buff_size; i++)
    {
        cout << buff[i] << "\n";
    }
    
    return 0;
}