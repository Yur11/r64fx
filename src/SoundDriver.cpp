#include "SoundDriver.hpp"
#include <new>
#include <iostream>

using namespace std;

#include "SoundDriver_Stub.cxx"
#ifdef R64FX_USE_JACK
#include "SoundDriver_Jack.cxx"
#endif//R64FX_USE_JACK


namespace r64fx{

SoundDriver* SoundDriver::newInstance(SoundDriver::Type type)
{
    SoundDriver* driver = nullptr;
    switch(type)
    {
        case SoundDriver::Type::Stub:
        {
            cout << "stub!\n";
            driver = new(std::nothrow) SoundDriver_Stub;
            break;
        }
        
        case SoundDriver::Type::Jack:
        {
            auto d = new(std::nothrow) SoundDriver_Jack;
            if(d && !d->isGood())
                delete d;
            else
                driver = d;
            break;
        }

        default:
        {
            break;
        }
    }

    return driver;
}


void SoundDriver::deleteInstance(SoundDriver* driver)
{
    driver->disable();
    delete driver;
}

}//namespace r64fx
