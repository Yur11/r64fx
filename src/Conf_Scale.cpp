#include "Conf_Scale.hpp"

namespace{
    float g_Scale = 1.0f;
}

namespace r64fx{
namespace Conf{

void setScale(float scale) { g_Scale = scale; }
float Scale() { return g_Scale; }

}//namespace Conf
}//namespace r64fx
