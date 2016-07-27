#include "Host.hpp"
#include "HostView.hpp"
#include "Timer.hpp"
#include "sleep.hpp"

#include "Player.hpp"

namespace r64fx{

int Host::exec()
{
    m_view = new HostView(this);

    while(m_is_running)
    {
        long sleep_time = Timer::runTimers();
        if(sleep_time <= 0 || sleep_time > 100)
            sleep_time = 100;
        sleep_microseconds(sleep_time);
    }

    delete m_view;
    return 0;
}


void Host::exit()
{
    m_is_running = false;
}


void Host::requestExit()
{
    exit();
}


void Host::openPlayer()
{
    if(!m_player)
    {
        m_player = new Player;
    }
    else
    {
        m_player->showView();
    }
}

}//namespace r64fx