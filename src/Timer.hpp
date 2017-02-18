#ifndef R64FX_TIMER_HPP
#define R64FX_TIMER_HPP

namespace r64fx{

class TimerThreadId;

class Timer{
    void* m = nullptr;

    Timer(const Timer&){}

public:
    Timer(unsigned long interval = 0);

    /* When in need to delete the timer from within its own callback use suicide(). */
    ~Timer();

    bool isGood() const;

    void setInterval(unsigned long interval); //in microseconds

    unsigned long interval() const;

    void onTimeout(void (*callback)(Timer* timer, void* arg), void* arg = nullptr);

    void start(unsigned long start_delay = 0);

    void stop();

    bool isRunning();

    void suicide();

    static TimerThreadId* reserveThreadId();

    static void freeThreadId(TimerThreadId* thread_id);

    static unsigned long runTimers(TimerThreadId* thread_id = nullptr);

    /* Cleanup after timers that have committed suicide. */
    static void cleanup();
};

}//namespace r64fx

#endif//R64FX_TIMER_HPP
