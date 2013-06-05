#include "Config.h"
#include <string>
#include <map>
#include <vector>
#include <dirent.h>

#include <iostream>

using namespace std;

namespace r64fx{

map<string, string> configs;
    
void append_missing_slash(string &str)
{
    if(str.back() != '/') str.push_back('/');
}
    
    
void create_default_config(FILE* file)
{
    
}
    
   
vector<string> drop_comments_split_into_lines(string str)
{
    str.push_back('\n');
    
    enum class State{
        Initial,
        InComment,
        InEntery
    };
    
    State state = State::Initial;
    
    vector<string> result;
    string line;
    
    for(auto ch : str)
    {
        switch(state)
        {
            case State::Initial:
            {
                if(ch == '#')
                {
                    state = State::InComment;
                }
                else if(ch == ' ' || ch == '\n' || ch == '\t') 
                {}
                else
                {
                    line.push_back(ch);
                    state = State::InEntery;
                }
                break;
            }
            
            case State::InComment:
            {
                if(ch == '\n')
                {
                    state = State::Initial;
                }
                break;
            }
            
            case State::InEntery:
            {
                if(ch == '\n')
                {
                    result.push_back(line);
                    line.clear();
                }
                else
                {
                    line.push_back(ch);
                }
                break;
            }
        }
    }
    
    return result;
}
    
    
vector<string> split(string str, char separator)
{
    vector<string> result;
    string item;
    for(char c: str)
    {
        if(c == separator)
        {
            result.push_back(item);
            item.clear();
        }
        else
        {
            item.push_back(c);
        }
    }
    
    if(!item.empty()) result.push_back(item);
    
    return result;
}


void strip_whitespace(string &str)
{
    while(!str.empty())
    {
        if(str.back() == ' ' || str.back() == '\t')
            str.pop_back();
        else
            break;
    }
    
    while(!str.empty())
    {
        if(str.front() == ' ' || str.front() == '\t')
            str.erase(0, 1);
        else
            break;
    }
}
    
    
bool Config::init()
{
    string home_dir = getenv("HOME");
    append_missing_slash(home_dir);
    
    string config_dir = home_dir + ".r64fx/";
    
    DIR* dir = opendir(config_dir.c_str());
    if(!dir)
    {
        if(errno == EACCES)
        {
            cerr << "r64fx: Config may exist, but permissions don't allow to access it!\nPlease fix that.";
        }
        else if(errno == EMFILE)
        {
            cerr << "r64fx: Too many open files!\n";
        }
        else if(errno == ENFILE)
        {
            cerr << "r64fx: System can't open more file!\n";
        }
        return false;
    }
    
    string gui_config_file = config_dir + "r64fx.conf";
    
    FILE* file = fopen(gui_config_file.c_str(), "r");
    if(!file && errno == EEXIST)
    {
        file = fopen(gui_config_file.c_str(), "rw");
        if(!file)
        {
            cerr << "r64fx: Failed to access gui.conf file!\n";
            return false;
        }
        
        create_default_config(file);
    }
    
    string text;
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    while(file_size--)
    {
        char c;
        fread(&c, 1, 1, file);
        text.push_back(c);
    }
    
    auto lines = drop_comments_split_into_lines(text);
    for(auto &line : lines)
    {
        auto segments = split(line, '=');
        if(segments.size() != 2)
        {
            cerr << "r64fx: Bad config!\n";
        }
        else
        {
            strip_whitespace(segments[0]);
            strip_whitespace(segments[1]);
            configs[segments[0]] = segments[1];
        }
    }
 
    fclose(file);
 
    return true;
}


std::string Config::item(std::string key)
{
    auto it = configs.find(key);
    if(it == configs.end())
    {
        return "";
    }
    
    return it->second;
}
    
}//namespace r64fx