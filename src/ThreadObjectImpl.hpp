#ifndef R64FX_THREAD_OBJECT_IMPL_HPP
#define R64FX_THREAD_OBJECT_IMPL_HPP

namespace r64fx{

class ThreadObjectImpl{
    friend class ThreadObjectCommLink_ImplEnd;
    ThreadObjectCommLink_ImplEnd*  m_comm_link  = nullptr;
    ThreadObjectIface*             m_iface      = nullptr;
    
public:
    ThreadObjectImpl(ThreadObjectIface* iface);
    
    virtual ~ThreadObjectImpl();
    
protected:
    void sendMessagesToIface(ThreadObjectMessage* msgs, int nmsgs);
    
private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg) = 0;
};

}//namespace r64fx

#endif//R64FX_THREAD_OBJECT_IMPL_HPP
