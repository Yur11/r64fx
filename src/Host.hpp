#ifndef R64FX_HOST_HPP
#define R64FX_HOST_HPP

#include "HostView.hpp"

namespace r64fx{

/* Main host controller. */
class Host : private HostViewControllerIface{
    HostView* m_view = nullptr;

    bool m_is_running = true;

public:
    int exec();

    void exit();

private:
    virtual void exitRequest();
};

}//namespace

#endif//R64FX_HOST_HPP