#include <iostream>

#include "Window_SDL2.hpp"
#include "Image.hpp"

using namespace std;
using namespace r64fx;

int main(int argc, char* argv[])
{
    Image img(64, 64, 3);

    cout << ( img.isGood() ) << "\n";

    return 0;
}
