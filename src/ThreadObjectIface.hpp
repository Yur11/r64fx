#ifndef R64FX_THREAD_OBJECT_IFACE_HPP
#define R64FX_THREAD_OBJECT_IFACE_HPP

namespace r64fx{

typedef ThreadObjectImpl* (*ThreadObjectDeploymentFun)(ThreadObjectIface* iface);
typedef void              (*ThreadObjectWithdrawalFun)();

class ThreadObjectIface{
    friend class ThreadObjectCommLink_IfaceEnd;
    ThreadObjectCommLink_IfaceEnd*  m_comm_link      = nullptr;
    ThreadObjectImpl*               m_deployed_impl  = nullptr;
    ThreadObjectIface*              m_parent         = nullptr;

protected:
    unsigned long m_flags = 0;

public:
    ThreadObjectIface(ThreadObjectIface* parent_iface = nullptr);

    virtual ~ThreadObjectIface();

    void setParent(ThreadObjectImpl* parent_iface);

    bool isDeployed() const;

    bool deploymentPending() const;

    bool withdrawalPending() const;

protected:
    void sendMessagesToImpl(ThreadObjectMessage* msgs, int nmsgs);

private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg) = 0;

    virtual ThreadObjectDeploymentFun* deploymentFun() = 0;

    virtual ThreadObjectWithdrawalFun* withdrawalFun() = 0;
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_IFACE_HPP
