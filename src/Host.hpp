#ifndef R64FX_HOST_HPP
#define R64FX_HOST_HPP

#include "HostModel.hpp"
#include "HostView.hpp"

namespace r64fx{

class Player;

/* Main host controller. */
class Host : private HostViewControllerIface{
    HostModel*  m_model  = nullptr;
    HostView*   m_view   = nullptr;

    bool m_is_running = true;

    Player* m_player = nullptr;

public:
    int exec();

    void exit();

    virtual void requestExit();

    virtual void openPlayer();
    
    virtual void ping();
};

}//namespace

#endif//R64FX_HOST_HPP