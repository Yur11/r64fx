#include <iostream>
#include "JackAudioEngine.h"


using namespace std;
using namespace r64fx;


int main()
{
    JackAudioEngine engine("r64fx");
    engine.createOutputPort("out_bla");
    
    char c;
    cin >> c;
    
    return 0;
}