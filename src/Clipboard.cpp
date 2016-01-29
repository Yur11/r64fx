#include "Clipboard.hpp"
#include <string>

using namespace std;

namespace r64fx{

bool operator==(ClipboardDataType a, ClipboardDataType b)
{
    return string(a.name()) == string(b.name());
}


bool ClipboardMetadata::contains(ClipboardDataType type)
{
    for(auto it=begin(); it!=end(); it++)
    {
        if(it->isGood() && *it == type)
        {
            return true;
        }
    }

    return false;
}

}//namespace r64fx