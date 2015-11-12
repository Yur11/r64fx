#include <iostream>
#include <string>

using namespace std;

int main()
{
    string str;
    char ch = 0;
    while(fread(&ch, sizeof(char), 1, stdin))
    {
        if(ch == '\n')
        {
            cout << '"' << str << "\\n\"\n";
            str.clear();
        }
        else
        {
            str.push_back(ch);
        }
    }
    if(!str.empty())
    {
        cout << '"' << str << "\\n\"\n";
    }
    cout << "\"\\n\"\n";

    return 0;
}
