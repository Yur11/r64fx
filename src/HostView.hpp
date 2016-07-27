#ifndef R64FX_HOST_VIEW_HPP
#define R64FX_HOST_VIEW_HPP

namespace r64fx{

class HostViewPrivate;
class HostViewControllerIface;

class HostView{
    HostViewPrivate* p = nullptr;

public:
    HostView(HostViewControllerIface* controller_iface);

    ~HostView();
};


class HostViewControllerIface{
public:
    virtual void requestExit() = 0;

    virtual void openPlayer() = 0;
};

}//namespace r64fx

#endif//R64FX_HOST_VIEW_HPP