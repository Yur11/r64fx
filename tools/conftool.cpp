#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        cerr << argv[0] << ": Insufficient input!\n" << argc << "\n";
        return 1;
    }
    
    string cxx = argv[1];
    string src_dir = argv[2];
    string subdir = argv[3];
    string cpp_file_list = argv[4];
    cpp_file_list.push_back(' ');
    string output_text;
    
    string str;
    vector<string> cpp_files;
    for(char ch : cpp_file_list)
    {
        if(ch == ' ' || ch == '\t' || ch == '\n')
        {
            if(!str.empty())
            {
                cpp_files.push_back(str);
                str.clear();
            }
        }
        else
        {
            str.push_back(ch);
        }
    }

    vector<string> o_files;
    
    string gen_dep_str = cxx + " -MM ";
    
    for(auto &cpp_file : cpp_files)
    {
        string suffix;
        for(unsigned int i=cpp_file.size()-1; i>=0; i--)
        {
            if(cpp_file[i] == '.')
            {
                suffix = cpp_file.substr(i);
                break;
            }
        }
        
        string o_file;
        if(suffix.empty())
            o_file = cpp_file; 
        else
            o_file = cpp_file.substr(0, cpp_file.size() - suffix.size());
        o_file += ".o";
        
        o_files.push_back(o_file);
        
        output_text += subdir + "/";
        
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
        output_text += outstr;

        output_text += "    echo \"Building    " + subdir + "/" + o_file + "\"\n"; 
        output_text += "    " + cxx + " -c " + path + " -o " + subdir + "/" + o_file;
        
        output_text += "\n\n";    
        
        pclose(pipe);
    }
    
    output_text += subdir + "_object_files = ";
    for(auto o_file : o_files)
    {
        output_text += " " + subdir + "/" + o_file;
    }
    output_text += "\n\n";
    
    string output_file_name = subdir + ".make_rules";
    auto file = fopen(output_file_name.c_str(), "w+");
    if(!file)
    {
        cerr << argv[0] << " Failed to open " << output_file_name << " for writing!\n";
        return 1;
    }
    
    fwrite(output_text.c_str(), 1, output_text.size(), file);
    
    fclose(file);
    
    return 0;
}