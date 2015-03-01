#ifndef R64FX_SHARED_READ_TEXT_HPP
#define R64FX_SHARED_READ_TEXT_HPP

#include <string>
#include <iostream>

namespace r64fx{
    
inline bool read_text_file(const std::string &path, std::string &text)
{
    using namespace std;
    
    auto file = fopen(path.c_str(), "r");
    if(!file)
    {
        cerr << "read_text_file: " << path << "\nFailed to open file!\n";
        return false;
    }
    
    char ch;
    while(fread(&ch, 1, 1, file) == 1)
    {
        text.push_back(ch);
    }
    
    if(fclose(file) != 0)
    {
        cerr << " read_text_file: " << path << "\nfclose() failed!\n";
        return false;
    }
    return true;
}
    
}//namespace r64fx

#endif//R64FX_SHARED_READ_TEXT_HPP