#include "AudioClipWidget.h"

#ifdef DEBUG
#include <assert.h>
#include <iostream>
using namespace std;
#endif//DEBUG

namespace r64fx{
   
AudioClipWidget::AudioClipWidget(AudioData* audio_data, Widget* parent)
: Widget(parent)
, p(4)
, dwp(4)
, audio_data(audio_data)
{
}


void AudioClipWidget::setAudioData(AudioData* audio_data)
{
    this->audio_data = audio_data;
}


void AudioClipWidget::updateWaveform()
{
#ifdef DEBUG
    assert(audio_data != nullptr);
#endif//DEBUG

    if(waveform_tex)
    {
        delete waveform_tex;
        waveform_tex = nullptr;
    }
    
    const int tex_size = 256;
    float tex_data[tex_size];
    
    audio_data->calculateSummaryStereo(tex_data, tex_size);
    
    waveform_tex = Texture1D::loadBaseLevelData(
        tex_data,
        (tex_size/2) * sizeof(float),
        (tex_size/2), 1, GL_FLOAT, GL_RG32F, GL_RG
    );
}


void AudioClipWidget::appearanceChangeEvent()
{    
    auto r = projectedRect();
    
    float p_data[16] = {
        r.left, r.top,
        r.right, r.top,
        r.right, r.bottom,
        r.left, r.bottom,
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p.bindBuffer();
    p.setData(p_data);
    p.unbindBuffer();
    
    
    float dwp_data[16] = {
        r.left, r.top,
        r.right, r.top,
        r.left, r.bottom,
        r.right, r.bottom,

        0.0, 0.0, 1.0, 1.0,
        
        0.0, 1.0, 0.0, 1.0
    };
    
    dwp.bindBuffer();
    dwp.setData(dwp_data);
    dwp.unbindBuffer();
}
    

void AudioClipWidget::render()
{
    p.setColor(0.0, 0.7, 0.0, 1.0);
    p.useNoTexture();
    p.bindArray();
    p.render(GL_LINE_LOOP);
    p.unbindArray();
    
    dwp.enable();
    dwp.setTexture(waveform_tex->glName());
    dwp.bindArray();
    dwp.render(GL_TRIANGLE_STRIP);
    dwp.unbindArray();
    
    Painter::enable();
}
    
}//namespace r64fx