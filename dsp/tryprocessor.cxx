#include <iostream>
#include "Processor.h"

using namespace std;
using namespace r64fx;

class MyProcessor : public Processor{
public:
    MyProcessor()
    {
        Processor::block_count = 1;
        Processor::block_size = 20;
    }
    
    virtual void render(Assembler &as){}
};

int main()
{
    MyProcessor mp;
    
    auto slot1 = mp.getSlot();
    auto slot2 = mp.getSlot();
    auto slot3 = mp.getSlot();
    auto slot4 = mp.getSlot();
    auto slot5 = mp.getSlot();
    auto slot6 = mp.getSlot();
    
    mp.debugSlots();
    cout << "------\n";
    
    mp.freeSlot(slot4);
    mp.freeSlot(slot6);

    mp.debugSlots();
    cout << "------\n";
    
    auto slot7 = mp.getSlot();
    
    mp.debugSlots();
    cout << "------\n";
    
    return 0;
}
