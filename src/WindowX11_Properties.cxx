/* To be included in WindowX11.cpp */

namespace{

bool get_window_property(
    ::Window  xwindow,
    Atom      property,
    bool      delete_property,

    unsigned long   &out_nitems,
    unsigned char*  &out_data,
    int             &format
)
{
    unsigned long   bytes_after;
    Atom            out_type;

    /* Get property length. */
    int result = XGetWindowProperty(
        g_display, xwindow, property,
        0, 0, False,
        AnyPropertyType,
        &out_type, &format, &out_nitems, &bytes_after,
        &out_data
    );

    if(result != Success)
    {
        return false;
    }

    if(bytes_after > 0)
    {
        int size = bytes_after;
        while(size % 4)
            size++;
        size /= 4;

        result = XGetWindowProperty(
            g_display, xwindow, property,
            0, size, delete_property,
            AnyPropertyType,
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

}//namespace