#include <iostream>
#include "ThreadObjectIface.hpp"
#include "ThreadObjectImpl.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

#define R64FX_TEST(testname)\
    case testname:\
    {\
        if(msg.value() != m_num)\
        {\
            cout <<  #testname ": Got:" << msg.value() << ", Expected: " << m_num << "\n";\
            g_failed = true;\
            return;\
        }\
        break;\
    }


using namespace std;
using namespace r64fx;

long g_test_depth = 0;
bool g_failed = false;

enum{
    SetNum,
    AddNum,
    SubNum
};

enum{
    NumSet,
    NumAdded,
    NumSubbed
};


class TestObjImpl : public ThreadObjectImpl{
    bool m_thread_running = true;
    unsigned long m_num = 0;

public:
    TestObjImpl(ThreadObjectIface* iface, ThreadObjectManagerImpl* manager_impl)
    : ThreadObjectImpl(iface, manager_impl)
    {
        
    }

private:
    virtual void messageFromIfaceRecieved(const ThreadObjectMessage &msg)
    {
        unsigned long num = msg.value();
        switch(msg.key())
        {
            case SetNum:
            {
                m_num = num;
                ThreadObjectMessage response_msg(NumSet, m_num);
                sendMessagesToIface(&response_msg, 1);
                break;
            }

            case AddNum:
            {
                m_num += num;
                ThreadObjectMessage response_msg(NumAdded, m_num);
                sendMessagesToIface(&response_msg, 1);
                break;
            }

            case SubNum:
            {
                m_num -= num;
                ThreadObjectMessage response_msg(NumSubbed, m_num);
                sendMessagesToIface(&response_msg, 1);
                break;
            }

            default:
            {
                cout << "Impl: Bad Message Key!\n";
                g_failed = true;
            }
        }
    }

    virtual void runThread()
    {
        while(m_thread_running)
        {
            readMessagesFromIface();
            sleep_nanoseconds(1000 * 1000);
        }
    }

    virtual void exitThread()
    {
        m_thread_running = false;
    }
};


class TestObjDeploymentAgent : public ThreadObjectDeploymentAgent{
    virtual ThreadObjectImpl* deployImpl(ThreadObjectIface* object_iface, ThreadObjectManagerImpl* manager_impl)
    {
        return new TestObjImpl(object_iface, manager_impl);
    }
};


class TestObjWithdrawalAgent : public ThreadObjectWithdrawalAgent{
    virtual void withdrawImpl(ThreadObjectImpl* impl)
    {
        delete impl;
    }
};


class TestObjIface : public ThreadObjectIface{
    unsigned long m_num = 0;

public:
    void run(TestObjIface* parent = nullptr)
    {
        g_test_depth++;
        deploy(parent, [](ThreadObjectIface* iface, void* arg){
            auto self = (TestObjIface*) arg;
            self->test();
        }, this);
    }

private:
    void test()
    {
        long act = (rand() % 16) + 1;
        switch(act)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                m_num = rand();
                ThreadObjectMessage msg(SetNum, m_num);
                sendMessagesToImpl(&msg, 1);
                break;
            }

            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            {
                unsigned long num = rand();
                m_num += num;
                ThreadObjectMessage msg(AddNum, num);
                sendMessagesToImpl(&msg, 1);
                break;
            }

            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            {
                unsigned long num = rand();
                m_num -= num;
                ThreadObjectMessage msg(SubNum, num);
                sendMessagesToImpl(&msg, 1);
                break;
            }

            default:
            {
                withdraw([](ThreadObjectIface* iface, void* arg){
                    g_test_depth--;
                }, this);
                break;
            }
        }
    }

    virtual void messageFromImplRecieved(const ThreadObjectMessage &msg)
    {
        switch(msg.key())
        {
            R64FX_TEST(NumSet)
            R64FX_TEST(NumAdded)
            R64FX_TEST(NumSubbed)

            default:
                break;
        }

        test();
    }

    virtual ThreadObjectDeploymentAgent* newDeploymentAgent()
    {
        return new TestObjDeploymentAgent;
    }

    virtual void deleteDeploymentAgent(ThreadObjectDeploymentAgent* agent)
    {
        delete agent;
    }

    virtual ThreadObjectWithdrawalAgent* newWithdrawalAgent()
    {
        return new TestObjWithdrawalAgent;
    }

    virtual void deleteWithdrawalAgent(ThreadObjectWithdrawalAgent* agent)
    {
        delete agent;
    }
};


bool gate1 = false;
bool gate2 = false;
bool gate3 = false;

#define R64FX_CHECK_OBJ(num, obj, pending, deployed) { if(!check_obj(num, obj, #obj, pending, deployed)) return false; }

bool check_obj(const char* num, const TestObjIface &obj, const char* name, bool pending, bool deployed)
{
    if(obj.isPending() != pending)
    {
        cout << num << ": Object " << name << " should" << (pending ? "" : " not") << " be pending!\n";
        return false;
    }
    if(obj.isDeployed() != deployed)
    {
        cout << num << ": Object " << name <<  " should" << (deployed ? "" : " not") << " be deployed!\n";
        return false;
    }
    return true;
}


bool test_deployment()
{
    /* Deploy One */
    TestObjIface a;
    R64FX_CHECK_OBJ("1", a, false, false);

    gate1 = false;
    a.deploy(nullptr, [](ThreadObjectIface* iface, void* arg){ gate1 = true; });
    R64FX_CHECK_OBJ("2", a, true, false);
    while(!gate1)
    {
        long timeout = Timer::runTimers();
        sleep_nanoseconds(timeout);
    }
    R64FX_CHECK_OBJ("3", a, false, true);

    /* Deploy Child */
    TestObjIface b;
    R64FX_CHECK_OBJ("4", b, false, false);
    
    gate1 = false;
    b.deploy(&a, [](ThreadObjectIface* iface, void* arg){ gate1 = true; });
    R64FX_CHECK_OBJ("5", b, true, false);
    while(!gate1)
    {
        long timeout = Timer::runTimers();
        sleep_nanoseconds(timeout);
    }
    R64FX_CHECK_OBJ("6", b, false, true);

    /* Withdraw Child */
    gate1 = false;
    b.withdraw([](ThreadObjectIface* iface, void* arg){ gate1 = true; });
    R64FX_CHECK_OBJ("7", b, true, true);
    while(!gate1)
    {
        long timeout = Timer::runTimers();
        sleep_nanoseconds(timeout);
    }
    R64FX_CHECK_OBJ("8", b, false, false);

    /* ReDeploy Child */
    gate1 = false;
    b.deploy(&a, [](ThreadObjectIface* iface, void* arg){ gate1 = true; });
    R64FX_CHECK_OBJ("9", b, true, false);
    while(!gate1)
    {
        long timeout = Timer::runTimers();
        sleep_nanoseconds(timeout);
    }
    R64FX_CHECK_OBJ("10", b, false, true);

    /* Deploy More Children */
    TestObjIface c, d, e;
    c.deploy(&a, [](ThreadObjectIface* iface, void* arg){ gate1 = true; });
    d.deploy(&b, [](ThreadObjectIface* iface, void* arg){ gate2 = true; });
    e.deploy(&b, [](ThreadObjectIface* iface, void* arg){ gate3 = true; });

    gate1 = gate2 = gate3 = false;
    while((!gate1) && (!gate2) && (!gate3))
    {
        long timeout = Timer::runTimers();
        sleep_nanoseconds(timeout);
    }
    R64FX_CHECK_OBJ("11.c", c, false, true);
    R64FX_CHECK_OBJ("11.d", d, false, true);
    R64FX_CHECK_OBJ("11.e", e, false, true);

    /* Withdraw All */
    gate1 = false;
    a.withdraw([](ThreadObjectIface* iface, void* arg){ gate1 = true; });
    R64FX_CHECK_OBJ("12.a", a, true, true);
    R64FX_CHECK_OBJ("12.b", b, true, true);
    R64FX_CHECK_OBJ("12.c", c, true, true);
    R64FX_CHECK_OBJ("12.d", d, true, true);
    R64FX_CHECK_OBJ("12.e", e, true, true);
    while(!gate1)
    {
        long timeout = Timer::runTimers();
        sleep_nanoseconds(timeout);
    }
    R64FX_CHECK_OBJ("13.a", a, false, false);
    R64FX_CHECK_OBJ("13.b", b, false, false);
    R64FX_CHECK_OBJ("13.c", c, false, false);
    R64FX_CHECK_OBJ("13.d", d, false, false);
    R64FX_CHECK_OBJ("13.e", e, false, false);

    return true;
}


bool test_messages()
{
    for(int i=0; i<32; i++)
    {
        TestObjIface obj;
        obj.run();

        while(g_test_depth > 0)
        {
            long timeout = Timer::runTimers();
            sleep_nanoseconds(timeout);
            if(g_failed)
            {
                return false;
            }
        }
    }
    return true;
}


int main()
{
    srand(time(nullptr));

    bool ok = true
        && test_deployment()
        && test_messages()
    ;

    if(ok)
    {
        cout << "OK!\n";
        return 0;
    }
    else
    {
        cout << "Fail!\n";
        return 1;
    }
}
