#include <iostream>
#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "Timer.hpp"
#include "sleep.hpp"

using namespace std;
using namespace r64fx;


struct TestObjectImpl : public ThreadObjectImpl{
    TestObjectImpl(int num, ThreadObjectIface* iface) : ThreadObjectImpl(iface)
    {
        cout << "1: " << num << " -> " << iface << " TestObjectImpl\n";
    }

    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        
    }
};


struct TestObjectDeploymentAgent : ThreadObjectDeploymentAgent{
    int num = 0;

    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* public_iface)
    {
        return new TestObjectImpl(num, public_iface);
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
    int m_num = 0;

public:
    TestObject(int num)
    : m_num(num)
    {
        
    }

private:
    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg)
    {
        
    }

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent()
    {
        auto agent = new TestObjectDeploymentAgent;
        agent->num = m_num;
        return agent;
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


TestObject tobj1(1);
TestObject tobj2(2);
TestObject tobj3(3);
TestObject tobj4(4);
TestObject tobj5(5);

int main()
{
    tobj2.deploy(&tobj1);
    tobj3.deploy(&tobj1);
    tobj4.deploy(&tobj2);
    tobj5.deploy(&tobj2);

    tobj1.deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
        cout << "Deployed!\n";
    });
    
    bool running = true;
    while(running)
    {
        Timer::runTimers();
        sleep_microseconds(1500);
    }
    
    return 0;
}
