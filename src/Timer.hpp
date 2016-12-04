#ifndef R64FX_TIMER_HPP
#define R64FX_TIMER_HPP

namespace r64fx{

class Timer{
    void* m = nullptr;

    Timer(const Timer&){}

public:
    Timer(long interval = 0);

    /* When in need to delete the timer from within it's own callback call suicide() before deleting. */
    ~Timer();

    bool isGood() const;

    void setInterval(long interval); //in microseconds

    long interval() const;

    void onTimeout(void (*callback)(Timer* timer, void* arg), void* arg);

    void start();

    void stop();

    bool isRunning();

    void suicide();

    static int runTimers();

    /* Cleanup after timers that have committed suicide. */
    static void cleanup();
};

}//namespace r64fx

#endif//R64FX_TIMER_HPP
