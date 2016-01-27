#include "Clipboard.hpp"
#include <string>

using namespace std;

namespace r64fx{

bool operator==(ClipboardDataType a, ClipboardDataType b)
{
    return string(a.name()) == string(b.name());
}

}//namespace r64fx