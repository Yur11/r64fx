#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "LinkedList.hpp"
#include "SignalNode.hpp"
#include "SignalNodeConnection.hpp"
#include "SoundDriver.hpp"
#include "MidiDispatcher.hpp"

namespace r64fx{


class SignalGraph{
    friend class SignalNodeClass;

    SoundDriver*                      m_driver       = nullptr;
    SoundDriverIOStatusPort*          m_status_port  = nullptr;
    LinkedList<SignalNodeClass>       m_node_classes;
    LinkedList<SignalNodeConnection>  m_connections;

    int   m_buffer_size      = 0.0f;
    float m_sample_rate      = 0.0f;
    float m_sample_rate_rcp  = 0.0f;

public:
    SignalGraph(SoundDriver* driver);

    SoundDriver* soundDriver() const;

    void addNodeClass(SignalNodeClass* node_class);

    SignalNodeConnection* newConnection(SignalNode* dst, SignalSink* dst_port, SignalNode* src, SignalPort* src_port);

    bool process();

private:
    int bufferSize() const;

    float sampleRate() const;

    float sampleRateReciprocal() const;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP