/* To be included in WindowX11.cpp */

namespace{

void get_dnd_type_list(const long* msg_data, vector<Atom> &types)
{
    ::Window source = msg_data[0];
    if(source == None)
    {
        cerr << "Source is none!\n";
        return;
    }

    if(msg_data[1] & 1)
    {
        cout << "More types!\n";

    }
    else
    {
        cout << "Less types!\n";

        for(int i=2; i<5; i++)
        {
            if(msg_data[i] != None)
            {
                types.push_back(msg_data[i]);
            }
        }
    }
}

}//namespace