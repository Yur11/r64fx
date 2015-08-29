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

const int subst_iteration_limit = 1024;

struct Rule{
    vector<string> deps;
    string body;
};

struct Rules : public map<string, Rule*> {} rules;

struct Variables : public map<string, string> {} variables;

vector<string> include_dirs;

const unsigned int buff_size = 1024 * 16;
char buff[buff_size];

string pwd()
{
    return getcwd(buff, buff_size);
}


string strip_extra_chars(string str)
{
    while(str.front() == ' ' || str.front() == '\t' || str.front() == '\n' )
        str = str.substr(1);
    while(str.back() == ' ' || str.back() == '\t' || str.back() == '\n' )
        str.pop_back(); 
    
    return str;
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


string read_included_file(const string &name)
{
    string file_name = strip_extra_chars(name);
    for(auto &dir : include_dirs)
    {
        auto path = dir + file_name;
        if(file_exists(path))
        {
            return read_text_file(path) + "\n";
        }
    }
    
    return "";   
}


void init_include_dirs()
{
    include_dirs.push_back(pwd());
    if(file_exists("SRC_DIR"))
    {
        string src_dir = strip_extra_chars(read_text_file("SRC_DIR"));
        if(!src_dir.empty())
            include_dirs.push_back(src_dir);
    }
    
    for(auto &dir : include_dirs)
    {
        if(dir.back() != '/')
            dir.push_back('/');        
    }
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


bool line_is_include_directive(const string &line, string &key)
{
    string extra_chars;
    
    enum class State{
        Initial,
        GotAtSign,
        GotSquare,
        Done
    };
    
    State state = State::Initial;
    
    for(char ch : line)
    {
        if(ch == '\n')
        {
            cerr << "Unexpected newline!\n";
            exit(1);
        }
        
        switch(state)
        {
            case State::Initial:
            {
                if(ch == ' ' || ch == '\t')
                    continue;
                else if(ch == '@')
                    state = State::GotAtSign;
                else
                    return false;
                break;
            }
            
            case State::GotAtSign:
            {
                if(ch == '[')
                    state = State::GotSquare;
                else
                {
                    cerr << "Unexpected symbol " << ch << " !\n";
                    exit(1);
                }
                break;
            }
            
            case State::GotSquare:
            {
                if(ch == ']')
                    state = State::Done;
                else
                    key.push_back(ch);
                break;
            }
            
            case State::Done:
            {
                if(ch == ' ' || ch == '\t')
                    continue;
                else
                    extra_chars.push_back(ch);
                break;
            }
        }
    }
    
    if(state == State::Done)
    {
        if(!extra_chars.empty() && extra_chars[0] != '#')
        {
            cerr << "maketool: Warning, extra characters after include statement!\n";
            cerr << "    " << extra_chars << "\n";
        }
        
        if(key.empty())
        {
            cerr << "maketool: Empty include @[] statement!\n";
            exit(0);
        }
        
        return true;
    }
    else if(state == State::Initial)
    {
        return false;
    }
    else
    {
        cerr << "Include statement @[" << key <<  "... ends unexpectedly!\n";
        exit(1);
    }
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


void build_dep_list(const string &dep_text, vector<string> &dep_list)
{    
    string str;
    for(char ch : dep_text)
    {
        if(ch == ' ' || ch == '\t' || ch == '\n')
        {
            if(!str.empty())
            {
                dep_list.push_back(str);
                str.clear();
            }
        }
        else
        {
            str.push_back(ch);
        }
    }
    
    if(!str.empty())
        dep_list.push_back(str);
}


void rebuild_dep_lists()
{
    for(auto &rule : rules)
    {
        vector<string> new_deps;
        auto &deps = rule.second->deps;
        for(auto dep : deps)
        {
            build_dep_list(dep, new_deps);
        }
        
        if(!new_deps.empty())
            rule.second->deps = new_deps;
    }
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
            build_dep_list(dep_text, rule->deps);
            
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
                variables[name_text] = strip_extra_chars(read_text_file(strip_extra_chars(value_text.substr(1, value_text.size() - 1))));
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

void parse(const string &text)
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
            while(!str.empty() && (str.back() == ' ' || str.back() == '\t'))//Strip extra chars.
            {
                str.pop_back();
            }

            if(str.empty())
                continue;
            
            if(line_is_comment(str))
            {
                str.clear();
                continue;
            }
            
            if(str.back() == '\\')
            {
                //Concat with the next line.
                str.pop_back();
                continue;
            }
            
            int indent_level;
            if(line_has_good_characters(str, indent_level))
            {
                
                if(indent_level == 0)
                {
                    string key;
                    if(line_is_include_directive(str, key))
                    {
                        //Parse another file.
                        parse(read_included_file(key));
                    }
                    else
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
bool perform_substitutions(string &text)
{
    string new_text;
    
    enum class State{
        Initial,
        GotAtSign,
        GotCurly,
        GotSquare
    };
    
    auto state = State::Initial;
    
    bool substitutions_occurred = false;
    
    string key;
    for(unsigned int i=0; i<text.size(); i++)
    {
        char ch = text[i];
        
        switch(state)
        {
            case State::Initial:
            {
                if(ch == '@')
                {
                    state = State::GotAtSign;
                }
                else
                {
                    new_text.push_back(ch);
                }
                break;
            }
            
            case State::GotAtSign:
            {
                if(ch == '{')
                {
                    state = State::GotCurly;
                }
                else if(ch == '[')
                {
                    state = State::GotSquare;
                }
                else
                {
                    new_text.push_back('@');
                    new_text.push_back(ch);
                    state = State::Initial;
                }
                break;
            }
            
            case State::GotCurly:
            {
                if(ch == '}')
                {
                    /*Perform substitution.*/
                    if(key.empty())
                    {
                        cout << "Warning: Got empty @{} in text!\n";
                    }
                    else
                    {                 
                        new_text += variables[key];
                        key.clear();
                        
                        substitutions_occurred = true;
                    }
                    
                    state = State::Initial;
                }
                else if(ch == ' ' || ch == '\t')
                {
                    //Skip
                }
                else if(ch == '\n')
                {
                    cerr << "Unexpected newline!\n";
                    exit(1);
                }
                else
                {
                    key.push_back(ch);
                }
                break;
            }
            
            case State::GotSquare:
            {
                if(ch == ']')
                {
                    if(key.empty())
                    {
                        cout << "Warning: Got empty @[] in text!\n";
                    }
                    else
                    {                 
                        if(file_exists(key))
                            new_text += strip_extra_chars(read_text_file(strip_extra_chars(key)));
                        key.clear();
                        
                        substitutions_occurred = true;
                    }
                    
                    state = State::Initial;
                }
                else if(ch == ' ' || ch == '\t')
                {
                    //Skip
                }
                else if(ch == '\n')
                {
                    cerr << "Unexpected newline!\n";
                    exit(1);
                }
                else
                {
                    key.push_back(ch);
                }
                break;
            }
        }
    }
    
    if(state == State::GotCurly)
    {
        cout << "Unexpected end of line!\n";
        cout << text << "\n";
        exit(1);
    }
    
    text = new_text;
    
    return substitutions_occurred;
}


bool perform_substitutions(Rules &rules)
{
    bool substitutions_occurred = false;
    
    for(auto rule : rules)
    {
        auto name = rule.first;
        auto old_name = name;
        auto suboc = perform_substitutions(name);
        if(suboc)
        {
            rules.erase(old_name);
            rules[name] = rule.second;
            substitutions_occurred = true;
        }
        
        for(auto &dep : rule.second->deps)
        {
            if(perform_substitutions(dep))
                substitutions_occurred = true;
        }
        
        if(perform_substitutions(rule.second->body))
        {
            substitutions_occurred = true;
        }
    }
    
    return substitutions_occurred;
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


string find_common_prefix(const string &a, const string &b)
{
    string str;
    for(unsigned int i=0; i<a.size() && i<b.size(); i++)
    {
        if(a[i] == b[i])
            str.push_back(a[i]);
        else
            break;
    }
    
    /* Chop up to the last / */
    while(!str.empty() && str.back() != '/')
        str.pop_back();
    
    return str;
}


string format_time_file_path(string target_name)
{
    string str = find_common_prefix(pwd(), target_name);
    if(!str.empty())
    {
        str = target_name.substr(str.size());
    }
    return str + ".time";
}


void save_timestamp(string target_name, unsigned long timestamp)
{
    if(timestamp == 0)
        return;
    
    string name = format_time_file_path(target_name);
    touch_path(name);
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
    
    string name = format_time_file_path(target_name);
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
            if(dep.empty())
                continue;
            
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
    
    init_include_dirs();
    
    string text;
    char ch;
    while(fread(&ch, 1, 1, stdin) !=0)
    {
        text.push_back(ch);
    }
    
    text.push_back('\n');
    parse(text);
    
    int subst_iteration_count = 0;
    while(perform_substitutions(rules))
    {
        rebuild_dep_lists();
        if(subst_iteration_count > subst_iteration_limit)
        {
            cerr << "To many substitutions!\nAssuming a circular dependency is present!\nAborting!\n";
            exit(1);
        }
        subst_iteration_count++;
    }
        
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