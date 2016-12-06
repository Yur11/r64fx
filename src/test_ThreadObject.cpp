#include <iostream>
#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

using namespace std;
using namespace r64fx;

enum{
    Ping
};


struct TestObjectImpl : public ThreadObjectImpl{
    int m_num = 0;

    TestObjectImpl(int num, ThreadObjectIface* iface) : ThreadObjectImpl(iface)
    {
        m_num = num;
        cout << "1: " << m_num << " -> TestObjectImpl\n";
    }

    ~TestObjectImpl()
    {
        cout << "1: " << m_num << " <- TestObjectImpl\n";
    }

    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        if(msg.key() == Ping)
        {
            cout << "Ping: " << (int)msg.value() << "\n";
        }
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
            sleep_nanoseconds(1500 * 1000);
        }
    }

    virtual void terminate()
    {
        m_running = false;
    }
};


class TestObject : public ThreadObjectIface{
    int m_num = 0;

public:
    TestObject(int num)
    : m_num(num)
    {
        
    }

    void ping(int num)
    {
        ThreadObjectMessage msg(Ping, num);
        sendMessagesToImpl(&msg, 1);
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


class Test{
    bool m_running = true;

    TestObject m_obj1 = 1;
    TestObject m_obj2 = 2;
    TestObject m_obj3 = 3;
    TestObject m_obj4 = 4;
    TestObject m_obj5 = 5;
    TestObject m_obj6 = 6;
    TestObject m_obj7 = 7;
    TestObject m_obj8 = 8;
    TestObject m_obj9 = 9;

    void deployBunch()
    {
        cout << "Deploy Bunch\n";

        m_obj2.deploy(&m_obj1);
        m_obj3.deploy(&m_obj1);
        m_obj4.deploy(&m_obj1);
        m_obj5.deploy(&m_obj2);
        m_obj6.deploy(&m_obj2);
        m_obj7.deploy(&m_obj3);
        m_obj8.deploy(&m_obj7);
        m_obj9.deploy(&m_obj5);
        m_obj1.deploy(nullptr, [](ThreadObjectIface* iface, void* arg){
            auto m = (Test*) arg;
            m->m_obj1.ping(123);
            m->m_obj2.ping(123);
            m->m_obj3.ping(123);
            m->m_obj4.ping(123);
            m->m_obj5.ping(123);
            m->m_obj6.ping(123);
            m->m_obj7.ping(123);
            m->m_obj8.ping(123);
            m->m_obj9.ping(123);
            m->withdrawBunch();
        }, this);
    }

    void withdrawBunch()
    {
        cout << "Withdraw Bunch\n";
        m_obj1.withdraw([](ThreadObjectIface* iface, void* arg){
            auto m = (Test*) arg;
            m->m_running = false;
        }, this);
    }

public:
    int exec()
    {
        deployBunch();

        while(m_running)
        {
            Timer::runTimers();
            sleep_nanoseconds(1500 * 1000);
        }
        Timer::cleanup();

        return 0;
    }
};


int main()
{
    Test test;
    return test.exec();
}
