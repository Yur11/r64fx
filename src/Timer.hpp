#ifndef R64FX_TIMER_HPP
#define R64FX_TIMER_HPP

namespace r64fx{

class Timer{
    void* m = nullptr;

    Timer(const Timer&){}

public:
    Timer(long interval = 0);

    ~Timer();

    void setInterval(long interval); //in microseconds

    long interval() const;

    void onTimeout(void (*callback)(Timer* timer, void* arg), void* arg);

    void start();

    void stop();

    bool isRunning();

    static int runTimers();
};

}//namespace r64fx

#endif//R64FX_TIMER_HPP