#include <iostream>
#include <iomanip>
#include <cmath>
#include "JackAudioEngine.h"
#include "AudioData.h"


using namespace std;
using namespace r64fx;


AudioData convolve(AudioData input, AudioData kernel)
{
    AudioData result(input.size());

    for(int i=kernel.size(); i<(int)input.size(); i++)
    {
        cout << "\r" << i << " / " << input.size();
        cout.flush();
        float sum = 0.0;
        for(int j=kernel.size()-1; j>=0; j--)
        {
            sum += input[i-j] * kernel[j];
        }
        result[i] = sum;
    }
    cout << "\n";

    return result;
}


AudioData gen_kernel()
{
    AudioData result(20000);
    for(int i=0; i<(int)result.size(); i++)
    {
        result[i] = 0.0;
    }

    result[0] = 1.0;
    result[250] = 0.025;
    result[500] = 0.05;
    result[9999] = 0.1;
    result[19999] = 0.2;

    return result;
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Give me an audio file!\n";
        return 1;
    }

//     JackAudioEngine engine("r64fx");

    auto data = AudioData::readFile(argv[1], 48000, 1024 * 64, [](void*, unsigned long int processed, unsigned long int total){
        cout << "\r" << int((double(processed) / double(total)) * 100) << "%";
        cout.flush();
        return true;
    });
    cout << "\n";

    cout << "convolving\n";
    auto kernel = gen_kernel();
    vector<AudioData> convolved_data;
    for(int i=0; i<(int)data.size(); i++)
    {
        convolved_data.push_back(convolve(data[i], kernel));
    }
    cout << "\nready\n";

    AudioData::saveFile("dump.wav", convolved_data, 48000);

//     char c;
//     cin >> c;
//
//     engine.runDebugAudioPlayer(convolved_data);
//     engine.activate();
//
//     cin >> c;

//     engine.deactivate();

    return 0;
}