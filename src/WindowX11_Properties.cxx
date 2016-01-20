/* To be included in WindowX11.cpp */

namespace{

bool get_window_property(
    ::Window  xwindow,
    Atom      property,
    Atom      expected_type,
    int       expected_format,
    bool      delete_property,

    Atom            &out_type,
    unsigned long   &out_nitems,
    unsigned char*  &out_data
)
{
    int             format;
    unsigned long   bytes_after;

    /* Get property length. */
    int result = XGetWindowProperty(
        g_display, xwindow, property,
        0, 0, False,
        expected_type,
        &out_type, &format, &out_nitems, &bytes_after,
        &out_data
    );

    if(result != Success)
    {
        return false;
    }

    if(format == expected_format && bytes_after > 0)
    {
        int size = bytes_after;
        while(size % 4)
            size++;
        size /= 4;

        result = XGetWindowProperty(
            g_display, xwindow, property,
            0, size, delete_property,
            expected_type,
            &out_type, &format, &out_nitems, &bytes_after,
            &out_data
        );

        return result == Success;
    }
    else
    {
        return false;
    }
}


bool get_window_text_property(
    ::Window  xwindow,
    Atom      property,
    Atom      expected_type,
    bool      delete_property,
    string    &out_text
)
{
    unsigned char* data = nullptr;
    unsigned long nitems = 0;
    Atom type;

    bool ok = get_window_property(xwindow, property, expected_type, 8, delete_property, type, nitems, data);
    if(ok && nitems > 0)
    {
        out_text = string((char*)data, nitems);
        XFree(data);
        return true;
    }
    else
    {
        return false;
    }
}


bool get_window_atom_list_property(
    ::Window  xwindow,
    Atom      property,
    Atom      expected_type,
    bool      delete_property,
    vector<Atom> &atoms
)
{
    unsigned char* data = nullptr;
    unsigned long nitems = 0;
    Atom type;

    bool ok = get_window_property(xwindow, property, expected_type, 32, delete_property, type, nitems, data);
    if(ok)
    {
        if(nitems > 0)
        {
            Atom* atoms_buff = (Atom*) data;
            for(int i=0; i<(int)nitems; i++)
            {
                atoms.push_back(atoms_buff[i]);
            }
            XFree(data);
        }
        return true;
    }
    else
    {
        return false;
    }
}

}//namespace