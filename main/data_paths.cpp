/** Directories where r64fx will look for the data. 
 *  Enteries must end with a slash!
 */
vector<string> data_paths = {
    "./",
    "./data/",             //For launching from the project root directory.
    "~/.r64fx/",
    "/usr/share/r64fx/",
};


/** @brief Find a file in a set of data directories.

    @return Function returns the full path to the file or an empty string
            if the file is not found.
 */
string find_path(string item)
{
    for(auto &prefix : data_paths)
    {
        string path = prefix + item;
        FILE* file = fopen(path.c_str(), "rb");
        if(file)
        {
            fclose(file);
            return path;
        }
    }

    return "";
}