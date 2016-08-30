#ifndef R64FX_SIGNAL_NODE_SAMPLER_HPP
#define R64FX_SIGNAL_NODE_SAMPLER_HPP

#include "SignalGraph.hpp"

namespace r64fx{

class SignalNode_Sampler : public SignalNode{
    float* m_data = nullptr;
    int m_frame_count = 0;
    int m_component_count = 0;
    float m_sample_rate = 0.0f;
    float m_sample_rate_rcp = 0.0;

    float m_start    = 0.0f;
    float m_stop     = 0.0f;
    float m_loop_in  = 0.0f;
    float m_loop_out = 0.0f;
    float m_playhead = 0.0f;
    float m_playhead_kahan = 0.0f;
    float m_pitch    = 1.0f;
    float m_gain     = 1.0f;

    unsigned int m_flags = 0;

    SignalSource* m_output = nullptr;
    int m_output_size = 0;

public:
    SignalNode_Sampler();

    virtual ~SignalNode_Sampler();

    void setData(float* data, int frame_count, int component_count, float sample_rate);

    void clear();

    int componentCount();

    void setStartTime(float start);

    float startPostion() const;

    void setStopPostion(float stop);

    float stopTime() const;

    void setLoopInTime(float loop_in);

    float loopInTime() const;

    void setLoopOutTime(float loop_out);

    float loopOutTime() const;

    void setPlayheadTime(float playhead);

    float playheadTime() const;

    void setPitch(float pitch);

    float pitch() const;

    void setGain(float gain);

    float gain() const;

    void play();

    void stop();

    bool isPlaying() const;

    void enableLoop();

    void disableLoop();

    bool isLooping() const;

    SignalSource* output(int channel);

    void resizeOutput(int size);

private:
    virtual void addedToGraph(SignalGraph* graph);

    virtual void aboutToBeRemovedFromGraph(SignalGraph* graph);

    virtual void processSample(int i);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_NODE_SAMPLER_HPP