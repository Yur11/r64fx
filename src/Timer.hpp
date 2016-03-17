#ifndef R64FX_TIMER_HPP
#define R64FX_TIMER_HPP

namespace r64fx{

class Timer{
    void* m = nullptr;

    Timer(const Timer&){}

public:
    Timer(long interval = 0);

    ~Timer();

    void setInterval(long interval); //in micoseconds

    long interval() const;

    void onTimeout(void (*callback)(Timer* timer, void* data), void* data);

    void start();

    void stop();

    bool isRunning();

    static int runTimers();
};


long current_time();

}//namespace r64fx

#endif//R64FX_TIMER_HPP