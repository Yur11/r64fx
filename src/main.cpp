#include "PlayerController.hpp"

#include "MainView.hpp"

using namespace std;
using namespace r64fx;

namespace r64fx{

bool g_running = true;

}//namespace r64fx


int main(int argc, char* argv[])
{
    MainView mv;
    mv.show();
    
    PlayerController p;
    p.exec();

    return 0;
}
