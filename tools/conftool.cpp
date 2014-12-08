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

using namespace std;

const string program_name = "conftool";

const string helptext = "Usage: " + program_name + " <compiler_command + flags > <src_dir> <sub_dir> <list_of_cpp_files>\n"
                        "Where:\n"
                        "       <src_dir> - Root source directory. \n"
                        "       <sub_dir> - A single subdirectory in the source directory. \n"
                        "       <list_of_cpp_files> - A whitespace separated list of files to process. \n"
;


bool write_text_file(const string &file_name, const string &text)
{
    auto file = fopen(file_name.c_str(), "w+");
    if(!file)
    {
        cerr << program_name << ": Failed to open " << file_name << " for writing!\n";
        return false;
    }
    fwrite(text.c_str(), 1, text.size(), file);
    fclose(file);
    return true;
}


void parse_list(const string &text, vector<string> &output_list)
{
    string str;
    for(char ch : text + " ")
    {
        if(ch == ' ' || ch == '\t' || ch == '\n')
        {
            if(!str.empty())
            {
                output_list.push_back(str);
                str.clear();
            }
        }
        else
        {
            str.push_back(ch);
        }
    }
}


int extract_suffix(const string &name, string &suffix)
{
    for(unsigned int i=name.size()-1; i>=0; i--)
    {
        if(name[i] == '.')
        {
            suffix = name.substr(i);
            return i;
        }
    }
}


int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        cerr << program_name << ": Bad input!\n";
        cerr << helptext << "\n";
        return 1;
    }
    
    string cxx             = argv[1];
    string src_dir         = argv[2];
    string subdir          = argv[3];
    string cpp_file_list   = argv[4];
    cpp_file_list.push_back(' ');
    
    string rules_text;
    string objects_text;

    /* Build a list of cpp files to process. */
    vector<string> cpp_files;
    parse_list(cpp_file_list, cpp_files);

    /* Generate rules for each of the cpp files and write a list of object files. */
    vector<string> o_files; //A list of all object files.
    string gen_dep_str = cxx + " -MM ";
    for(auto &cpp_file : cpp_files)
    {
        string suffix;
        extract_suffix(cpp_file, suffix);
        
        string o_file;
        if(suffix.empty()) 
        {
            //No suffix!
            o_file = cpp_file; 
        }
        else 
        {   
            //Drop existing suffix
            o_file = cpp_file.substr(0, cpp_file.size() - suffix.size());
        }
        o_file += ".o";
        
        o_files.push_back(o_file);
        
        rules_text += subdir + "/";
        
        string path = src_dir + "/" + subdir + "/" + cpp_file;
        string str = gen_dep_str + path;
        cout << "Configuring " << subdir << "/" << cpp_file << "\n";
        auto pipe = popen(str.c_str(), "r");
        if(!pipe)
        {
            cerr << argv[0] << " Failed to open pipe!\n";
            return 1;
        }

        string outstr;
        char ch;
        while(fread(&ch, 1, 1, pipe) == 1)
        {
            outstr.push_back(ch);
        }
        
        if(outstr.empty())
        {
            cerr << argv[0] << " Configuration failed!\n";
            return 1;
        }
        rules_text += outstr;

        rules_text += "    echo \"Building    " + subdir + "/" + o_file + "\"\n"; 
        rules_text += "    " + cxx + " -c " + path + " -o " + subdir + "/" + o_file;
        
        rules_text += "\n\n";    
        
        pclose(pipe);
    }
    
    if(!write_text_file(subdir + ".rules", rules_text))
    {
        return 1;
    }


    for(auto o_file : o_files)
    {
        objects_text += " " + subdir + "/" + o_file;
    }
    
    if(!write_text_file(subdir + ".objects", objects_text + "\n\n"))
    {
        return 1;
    }
    
    return 0;
}