/* 
    Copyright 2014 Yuri Klopovsky
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;


const int rule_indent_level = 4;

const int dir_flags = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

struct Rule{
    vector<string> deps;
    string body;
};

struct Rules : public map<string, Rule*> {} rules;

struct Variables : public map<string, string> {} variables;


const unsigned int buff_size = 1024 * 16;
char buff[buff_size];

string pwd()
{
    return getcwd(buff, buff_size);
}


unsigned long get_mod_time(const string &path)
{
    struct stat s;
    if(stat(path.c_str(), &s) == 0)
    {
        return s.st_mtime;
    }
    else
    {
        return 0;
    }
}


bool file_exists(const string &path)
{
    return get_mod_time(path) != 0;
}


string read_text_file(string path)
{
    string text;
    
    auto file = fopen(path.c_str(), "r");
    if(file)
    {
        char ch;
        while(fread(&ch, 1, 1, file) == 1)
        {
            text.push_back(ch);
        }
        fclose(file);
    }
    else
    {
        cerr << "Failed to read text file " << path << "\n!";
        exit(1);
    }
    
    return text;
}


bool line_is_comment(const string &str)
{
    for(char ch : str)
    {
        if(ch == ' ')
            continue;
        
        if(ch == '#')
            return true;
        else
            return false;
    }
    
    return false;
}


bool line_has_good_characters(const string &str, int &indent_level)
{
    indent_level = 0;
    unsigned int i=0;
    while(i<str.size())
    {
        char ch = str[i];
        if(ch == ' ')
            indent_level++;
        else if(ch == '\t')
            indent_level += rule_indent_level;
        else
            return true;
        i++;
    }
    
    return false;
}


string strip_extra_chars(string str)
{
    while(str.front() == ' ' || str.front() == '\t' || str.front() == '\n' )
        str = str.substr(1);
    while(str.back() == ' ' || str.back() == '\t' || str.back() == '\n' )
        str.pop_back(); 
    
    return str;
}


inline string file_deref(string str)
{
    return read_text_file(strip_extra_chars(str.substr(1, str.size() - 2)));
}


void process_entery(const string &header_text, const string &body_text, Rules &rules, Variables &variables)
{
    char ch;
    for(unsigned int i=0; i<header_text.size(); i++) 
    {
        ch = header_text[i];
        if(ch == ':')
        {
            /* Build rule. */
            auto rule = new Rule;
            
            string target_text = header_text.substr(0, i);
            while(!target_text.empty() && (target_text.back() == ' ' || target_text.back() == '\t'))
                target_text.pop_back();
            
            string dep_text;
            if(header_text.size() >= target_text.size() + 1)
            {
                dep_text = header_text.substr(i + 1);
            }
            dep_text.push_back(' ');
            string str;
            for(char ch : dep_text)
            {
                if(ch == ' ' || ch == '\t')
                {
                    if(!str.empty())
                    {
                        rule->deps.push_back(str);
                        str.clear();
                    }
                }
                else
                {
                    str.push_back(ch);
                }
            }
            
            rule->body = body_text;
            
            auto it = rules.find(header_text);
            if(it != rules.end())
                delete it->second;
            rules[target_text] = rule;
            
            return;
        }
        else if(ch == '=')
        {
            /* Variable definition. */
            
            string name_text = header_text.substr(0, i);
            while(!name_text.empty() && (name_text.back() == ' ' || name_text.back() == '\t'))
                name_text.pop_back();
            
            unsigned int j=i+1;
            while(j<header_text.size() && (header_text[j] == ' ' || header_text[j] == '\t'))
            {
                j++;
            }
            string value_text;
            if(j < header_text.size())
                value_text = header_text.substr(j) + "\n";
            
            value_text += body_text;
            
            /* Strip trailing witespace and newlines. */
            while(value_text.back() == ' ' || value_text.back() == '\t' || value_text.back() == '\n')
                value_text.pop_back();
            
            if(value_text.front() == '[' && value_text.back() == ']')
            {
                variables[name_text] = strip_extra_chars(file_deref(value_text));
            }
            else
            {
                variables[name_text] = value_text;
            }
            
            return;
        }
    }

    
    /* Without deps. */
    string str = header_text;
    while(!str.empty() && (str.back() == ' ' || str.back() == '\t'))
        str.pop_back();
    
    if(!str.empty())
    {
        auto it = rules.find(header_text);
        if(it != rules.end())
            delete it->second;
        
        auto rule = new Rule;
        rule->body = body_text;
        rules[str] = rule;
    }
}

void parse(const string &text, Rules &rules, Variables &variables)
{
    string str;//Current line.
    string header_text;
    string body_text;
    int lineno = 1;

    for(unsigned int index=0; index<text.size(); index++)
    {
        char ch = text[index];
        
        if(ch == '\n')
        {
            if(str.empty())
                continue;
            
            if(line_is_comment(str))
            {
                str.clear();
                continue;
            }
            
            while(str.back() == ' ' || str.back() == '\t')
                str.pop_back();
            
            if(str.back() == '\\')
                continue;
            
            int indent_level;
            if(line_has_good_characters(str, indent_level))
            {
                if(indent_level == 0)
                {
                    //Write results here!
                    if(!header_text.empty())
                    {
                        process_entery(header_text, body_text, rules, variables);
                    }
                    
                    header_text.clear();
                    body_text.clear();
                    
                    header_text = str;
                }
                else if(indent_level >= rule_indent_level)
                {
                    //Append to rule.
                    body_text += str.substr(rule_indent_level) + "\n";
                }
                else
                {
                    cout << lineno << ": Bad indent level of " << indent_level << " !\n";
                    exit(1);
                }
            }
            else
            {
            }
            
            str.clear();
            lineno++;
        }
        else
        {
            //Read next line.
            str.push_back(ch);
        }
    }
    
    if(!header_text.empty())
    {
        process_entery(header_text, body_text, rules, variables);
    }
}


/* Substitute variables in text. 
   Return true is substitutions occured.
 */
bool substitute_variables(string &text, const Variables &variables)
{
    enum class State{
        Initial,
        GotHash,
        GotBrace
    };
    
    auto state = State::Initial;
    
    bool substitutions_occurred = false;
    
    int sub_begin = 0, sub_end = 0;
    for(unsigned int i=0; i<text.size(); i++)
    {
        char ch = text[i];
        
        switch(state)
        {
            case State::Initial:
            {
                if(ch == '#')
                {
                    state = State::GotHash;
                    sub_begin = i;
                }
                break;
            }
            
            case State::GotHash:
            {
                if(ch == '{')
                {
                    state = State::GotBrace;
                }
                break;
            }
            
            case State::GotBrace:
            {
                if(ch == '}')
                {
                    sub_end = i;
                    
                    /*Perform substitution.*/
                    if(sub_begin + 2 == sub_end)
                    {
                        cout << "Warning: Got empty #{} in text!\n";
                    }
                    else
                    {
                        int size = sub_end - sub_begin - 2;
                        string key = text.substr(sub_begin + 2, size);
                        while(key.back() == ' ' || key.back() == '\t')
                            key.pop_back();
                        while(key.front() == ' ' || key.front() == '\t')
                            key = key.substr(1);

                        string subst_text = "";
                        
                        auto it = variables.find(key);
                        if(it != variables.end())
                        {
                            subst_text = it->second;
                        }
                        
                        text = text.substr(0, sub_begin) + subst_text + text.substr(sub_end + 1);
                        
                        substitutions_occurred = true;
                    }
                    
                    state = State::Initial;
                }
            }
        }
    }
    
    if(state == State::GotBrace)
    {
        cout << "Unexpected end of line!\n";
        cout << text << "\n";
        exit(1);
    }
    
    return substitutions_occurred;
}


void substitute_variables(Rules &rules, const Variables &variables)
{
    for(auto rule : rules)
    {
        auto name = rule.first;
        auto old_name = name;
        if(substitute_variables(name, variables))
        {
            rules.erase(old_name);
            rules[name] = rule.second;
        }
        
        for(auto &dep : rule.second->deps)
        {
            substitute_variables(dep, variables);
        }
        
        substitute_variables(rule.second->body, variables);
    }
}


/* Create directories that lead to the target if needed. */
void touch_path(string path)
{
    if(file_exists(path))
        return;
    
    vector<string> dirs;
    string dir;
    for(char ch : path)
    {
        if(ch == '/')
        {
            dirs.push_back(dir);
            dir.clear();
        }
        else
        {
            dir.push_back(ch);
        }
    }
    
    if(dirs.empty())
        return;
    
    string wd = pwd();
    
    for(auto d : dirs)
    {
        if(!file_exists(d))
        {
            if(mkdir(d.c_str(), dir_flags) != 0)
            {
                cout << "Failed to create path " << path << " !\n";
                cout << "Error while creating subdirectory " << d << " !\n";
                exit(1);
            }
        }
        
        if(chdir(d.c_str()) != 0)
        {
            cout << "Failed to create path " << path << " !\n";
            cout << "Error while entering subdirectory " << d << " !\n";
            exit(1);
        }
    }
    
    chdir(wd.c_str());
}


void save_timestamp(string target_name, unsigned long timestamp)
{
    if(timestamp == 0)
        return;
    
    string name = target_name + ".time";
    auto file = fopen(name.c_str(), "w+");
    if(!file)
    {
        cout << "Failed to save timestamp for target " << target_name << " !\n";
        exit(1);
    }
    
    if(fwrite(&timestamp, sizeof(timestamp), 1, file) != 1)
    {
        cerr << "save_timestamp(" << target_name << ", " << timestamp << ") fwrite failed!\n";
        exit(1);
    }
    
//     cout << "save_timestamp(" << target_name << ", " << timestamp << ")\n";
    
    fclose(file);
}


unsigned long read_timestamp(string target_name)
{
    unsigned long num = 0;
    
    string name = target_name + ".time";
    auto file = fopen(name.c_str(), "r");
    if(file)
    {
        if(fread(&num, sizeof(num), 1, file) != 1)
        {
            cerr << "read_timestamp(" << target_name << ") fread failed!\n";
            exit(1);
        }
        fclose(file);
    }
    
//     cout << "read_timestamp(" << target_name << ") = " << num << "\n";
    
    return num;
}


bool is_a_target(string path, const Rules &rules)
{
    return rules.find(path) != rules.end();
}


void execute_body(string text)
{
    
}


enum{
    TargetUp2Date,
    TargetUpdated
};

int make_target(string target, const Rules &rules)
{
    bool target_is_dir = (target.back() == '/');
    
    if(target_is_dir)
    {
        if(file_exists(target))
        {
            return TargetUp2Date;
        }
        else
        {
            touch_path(target);
            return TargetUpdated;
        }
    }
    else
    {
        touch_path(target);
    
        auto it = rules.find(target);
        assert(it != rules.end());
        auto &deps = it->second->deps;
        auto &body = it->second->body;
        
        bool deps_updated = false;
        for(auto &dep : deps)
        {
            if(is_a_target(dep, rules))
            {
                if(make_target(dep, rules) == TargetUpdated)
                {
                    deps_updated = true;
                }
            }
            else
            {
                unsigned long dep_mod_time = get_mod_time(dep);
                bool dep_exists = (dep_mod_time != 0);
                if(!dep_exists)
                {
                    cout << "Dependency " << dep << " of target " << target << " does not exist and is not built by this project!\n";
                    exit(1);
                }
                
                unsigned long dep_recorded_mod_time = read_timestamp(dep);
                if(dep_mod_time > dep_recorded_mod_time)
                {
                    deps_updated = true;
                    save_timestamp(dep, dep_mod_time);
                }
            }
        }
        
        if(file_exists(target) && !deps_updated)
        {
            if(read_timestamp(target) == 0)
                save_timestamp(target, get_mod_time(target));
            
            return TargetUp2Date;
        }
        else
        {
            int code = system(body.c_str());
            if(code != 0)
            {
                cout << "Failed to build target " << target << " !\n";
                cout << "Error code " << code << "\n";
                exit(1);
            }
            
            if(file_exists(target))
            {
                save_timestamp(target, get_mod_time(target)); //Only if target has actually been created.
            }
            
            return TargetUpdated;
        }
    }
}


void cleanup()
{
    for(auto rule : rules)
    {
        delete rule.second;
    }
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Give me a target to build!\n";
        return 1;
    }
    
    atexit(cleanup);
    
    string text;
    char ch;
    while(fread(&ch, 1, 1, stdin) !=0)
    {
        text.push_back(ch);
    }
    
    text.push_back('\n');
    parse(text, rules, variables);
    substitute_variables(rules, variables);
    
    if(is_a_target(argv[1], rules))
    {
        if(make_target(argv[1], rules) == TargetUp2Date)
        {
            cout << "Target " << argv[1] << " is up to date!\n";
        }
    }
    else
    {
        cout << "No rule for target " << argv[1] << " !\n";
    }
    
    return 0;
}