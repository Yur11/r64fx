#include <iostream>
#include <iomanip>
#include "JackAudioEngine.h"
#include "AudioData.h"


using namespace std;
using namespace r64fx;


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Give me an audio file!\n";
        return 1;
    }

    JackAudioEngine engine("r64fx");

    auto data = AudioData::readFile(argv[1], 48000, 1024 * 64, [](void*, unsigned long int processed, unsigned long int total){
        cout << "\r" << int((double(processed) / double(total)) * 100) << "%";
        cout.flush();
        return true;
    });
    cout << "\n";

    engine.runDebugAudioPlayer(data);
    engine.activate();

    char c;
    cin >> c;

    engine.deactivate();

    return 0;
}