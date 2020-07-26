#ifndef R64FX_MAIN_LOOP_HPP
#define R64FX_MAIN_LOOP_HPP

namespace r64fx{

class MainLoop{
    bool m_running = false;

public:
    void run();

    void stop();
};

}//namespace r64fx

#endif//R64FX_MAIN_LOOP_HPP