#include "Arg.hpp"

#include <iostream>

using namespace std;

namespace r64fx{
    
bool Arg::parse(int argc, char* argv[])
{
    if(argc < 2)
        return false;
    
    int i=1;
    for(;;)
    {
        if(i>=argc)
            break;
        
        string arg = argv[i];
        
        if(arg.size() > 3 && arg[0] == '-' && arg[1] == '-')
        {
            string str = arg.substr(2);
            string key, value;
            bool got_separator = false;
            for(int n=0; n<str.size(); n++)
            {
                if(got_separator)
                {
                    value.push_back(str[n]);
                }
                else
                {
                    if(str[n] == '=')
                    {
                        got_separator = true;
                        continue;
                    }
                    
                    key.push_back(str[n]);
                }
            }
            
            auto it = find(key);
            if(it == end())
            {
                cerr << argv[0] << ": Bad key " << key << "!\n";
                return false;
            }
            
            if(value.empty())
                value = "1";
                
            (*this)[key] = value;
        }
        else
        {
            cerr << argv[0] << ": Bad input " << arg << "!\n";
            return false;
        }
        
        i++;
    }
    
    return true;
}
    
}//namespace r64fx