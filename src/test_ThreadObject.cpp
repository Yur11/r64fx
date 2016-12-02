#include <iostream>
#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "Timer.hpp"
#include "sleep.hpp"

using namespace std;
using namespace r64fx;


struct TestObjectImpl : public ThreadObjectImpl{
    TestObjectImpl(ThreadObjectIface* iface) : ThreadObjectImpl(iface)
    {
        
    }

    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        
    }
};


struct TestObjectDeploymentAgent : ThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface)
    {
        return new TestObjectImpl(public_iface);
    }
};


struct TestObjectWithdrawalAgent : ThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl)
    {
        delete impl;
    }
};


struct TestObjectExecAgent : ThreadObjectExecAgent{
    bool m_running = true;

    virtual void exec()
    {
        while(m_running)
        {
            readMessagesFromIface();
            sleep_microseconds(1500);
        }
    }
};


class TestObject : public ThreadObjectIface{
private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg)
    {
        
    }

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent()
    {
        return new TestObjectDeploymentAgent;
    }

    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent)
    {
        delete agent;
    }

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent()
    {
        return new TestObjectWithdrawalAgent;
    }
    
    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent)
    {
        delete agent;
    }
    
    virtual ThreadObjectExecAgent* newExecAgent()
    {
        return new TestObjectExecAgent;
    }
    
    virtual void deleteExecAgent(ThreadObjectExecAgent* agent)
    {
        delete agent;
    }
};


TestObject tobj1;
TestObject tobj2;
TestObject tobj3;
TestObject tobj4;
TestObject tobj5;

int main()
{
    tobj1.deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        cout << "Deployed 1\n";
        tobj2.deploy(iface, [](ThreadObjectIface* iface, void* arg){
            cout << "Deployed 2\n";
            tobj3.deploy(iface, [](ThreadObjectIface* iface, void* arg){
                cout << "Deployed 3\n";
            });
            tobj4.deploy(iface, [](ThreadObjectIface* iface, void* arg){
                cout << "Deployed 4\n";
            });
        });
        tobj5.deploy(iface, [](ThreadObjectIface* iface, void* arg){
            cout << "Deployed 5\n";
        });
    });
    
    bool running = true;
    while(running)
    {
        Timer::runTimers();
        sleep_microseconds(1500);
    }
    
    return 0;
}
