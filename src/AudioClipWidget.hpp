#ifndef R64FX_MAIN_AUDIO_CLIP_WIDGET_H
#define R64FX_MAIN_AUDIO_CLIP_WIDGET_H

#include "gui/Widget.hpp"
#include "gui/Texture.hpp"
#include "gui/Painter.hpp"
#include "DenseWaveformPainter.hpp"
#include "AudioData.hpp"


namespace r64fx{
    
class AudioClipWidget : public Widget{
    Painter p;
    DenseWaveformPainter dwp;
    AudioData* audio_data;
    Texture1D* waveform_tex;
    
    
public:
    AudioClipWidget(AudioData* audio_data, Widget* parent = nullptr);
    
    void setAudioData(AudioData* audio_data);
    
    void updateWaveform();
    
protected:
    virtual void appearanceChangeEvent();
    
    virtual void render();    
};
    
}//namespace r64fx

#endif//R64FX_MAIN_AUDIO_CLIP_WIDGET_H