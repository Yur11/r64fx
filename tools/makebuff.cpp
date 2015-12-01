#include <iostream>
#include <string>

using namespace std;

string byte2str(unsigned char byte)
{
    static char ch[17] = "0123456789ABCDEF";
    string str;
    str.push_back(ch[byte >> 4]);
    str.push_back(ch[byte & 15]);
    return str;
}

int main()
{
    string str;
    char ch = 0;
    int i = 0;
    while(fread(&ch, sizeof(char), 1, stdin))
    {
        str += "0x";
        str += byte2str(ch);
        str += ",";
        if((i & 15) == 15)
            str += "\n";
        else
            str += " ";
        i++;
    }

    if(str.size() > 2)
    {
        str.pop_back();
        str.pop_back();
    }
    cout << "unsigned char buff[" << i << "] = {\n" << str << "\n};\n";

    return 0;
}
