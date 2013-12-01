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
    
    cout << '"';
    
    char ch;
    while(fread(&ch, 1, 1, file) == 1)
    {        
        if(ch == '\n')
        {
            cout << "\\n\"\n\"";
        }
        else if(ch == '\t')
            cout << "\\t";
        else if(ch == '\\')
            cout << "\\\\";
        else if(ch == '"')
            cout << "\\\"";
        else
            cout << ch;
    }
    
    cout <<  '"';
    
    return 0;
}