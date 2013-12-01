#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << argv[0] << ": Give me an input file!\n";
        return 1;
    }
    
    auto file = fopen(argv[1], "r");
    if(!file)
    {
        cerr << argv[0] << ": Failed to open file \"" << argv[1] << "\" !\n";
        return 2;
    }

    cout << hex;
    
    char ch;
    
    if(fread(&ch, 1, 1, file) == 1)
        cout << (ch>=16? "":" ") << "0x" << (unsigned int) ch;
    else
    {
        cerr << argv[0] << ": File \"" << argv[1] << "\" seems to be empty!\n";
        return 3;
    }
    
    int i=1;
    while(fread(&ch, 1, 1, file) == 1)
    {        
        cout << ',';
        i++;
        if(i==16)
        {
            cout << '\n';
            i=1;
        }
        else
            cout << ' ';
        cout << (ch>=16? "":" ") << "0x" << (unsigned int) ch;
    }
        
    return 0;
}