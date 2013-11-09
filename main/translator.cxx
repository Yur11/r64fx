namespace r64fx{

/** @brief A simple text translator. */
class Translator : public map<string, string>{
public:
    std::string operator()(std::string key_text)
    {
        auto it = find(key_text);
        if(it == end())
            return key_text;
        else
            return it->second;
    }

    void loadLanguage(string lang)
    {
        clear();
        for(auto &path : data_paths)
        {
            Filesystem::listDirectory<Translator*>(
                this,
                path + "translations/" + lang,
                [](Translator* translator, string prefix, Filesystem::Entery entery){
                    if(entery.name()[0] == '.')
                        return true;

                    auto path = prefix + entery.name();
                    FILE* file = fopen(path.c_str(), "r");
                    if(!file)
                    {
                        cerr << "Failed to open file \"" << path << "\" while loading translations!\n";
                        return true;
                    }

                    string text;
                    char ch;
                    while(fread(&ch, 1, 1, file) == 1)
                    {
                        text.push_back(ch);
                    }

                    while(!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\n'))
                    {
                        text.pop_back();
                    }

                    (*translator)[entery.name()] = text;

                    fclose(file);

                    return true;
                }
            );
        }
    }
} tr;

}//namespace r64fx