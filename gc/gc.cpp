#include "gc.h"
#include <vector>
#include <iostream>

using namespace std;

namespace r64fx{
    
vector<Disposable*> disposable_objects;


void Disposable::deleteLater()
{
    disposable_objects.push_back(this);
}


int gc::deleteGarbage()
{
    int obj_count = disposable_objects.size();
    for(auto obj : disposable_objects)
    {
        delete obj;
    }
    disposable_objects.clear();
    return obj_count;
}

    
}//namespace r64fx
