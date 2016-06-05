/* To be included in WindowX11.cpp */

namespace{


::Window dnd_source(const long* msg_data)
{
    return (::Window) msg_data[0];
}


void get_dnd_type_list(const long* msg_data, vector<Atom> &types)
{
//     if(msg_data[1] & 1)
//     {
//         bool ok = get_window_atom_list_property(
//             dnd_source(msg_data),
//             X11_Atom::XdndTypeList,
//             XA_ATOM,
//             false,
//             types
//         );
//
//         if(!ok)
//         {
//             cerr << "Failed to fetch dnd source types!\n";
//         }
//     }
//     else
//     {
//         for(int i=2; i<5; i++)
//         {
//             if(msg_data[i] != None)
//             {
//                 types.push_back(msg_data[i]);
//             }
//         }
//     }
}


void get_dnd_position(const long* msg_data, int &x, int &y)
{
    unsigned int pos = msg_data[2];
    x = (pos >> 16) & 0xFFFF;
    y = pos & 0xFFFF;
}


void request_all_dnd_positions(::Window drag_target, ::Window drag_source)
{
    XEvent xevent;
    auto &out = xevent.xclient;
    auto &msg_data = out.data.l;

    out.type         = ClientMessage;
    out.display      = g_display;
    out.window       = drag_source;
    out.format       = 32;
    out.message_type = X11_Atom::XdndStatus;

    msg_data[0] = drag_target;
    msg_data[1] = 3;
    msg_data[2] = 0;
    msg_data[3] = 0;
    msg_data[4] = None;

    if(!XSendEvent(g_display, drag_source, False, NoEventMask, &xevent))
    {
        cerr << "Failed to XdndStatus event!\n";
    }
}


void send_dnd_finished(::Window drag_target, ::Window drag_source, bool accept)
{
    XEvent xevent;
    auto &out = xevent.xclient;
    auto &msg_data = out.data.l;

    out.type         = ClientMessage;
    out.display      = g_display;
    out.window       = drag_source;
    out.format       = 32;
    out.message_type = X11_Atom::XdndFinished;

    msg_data[0] = drag_target;
    msg_data[1] = 0;
    msg_data[2] = 0;
    msg_data[3] = 0;
    msg_data[4] = 0;

    if(!XSendEvent(g_display, drag_source, False, NoEventMask, &xevent))
    {
        cerr << "Failed to XdndStatus event!\n";
    }
}

}//namespace

void WindowX11::startDrag(Window* drag_object, int anchor_x, int anchor_y)
{
    if(g_outgoing_drag_object)
        return;

    cout << "startDrag: " << drag_object << ", " << anchor_x << ", " << anchor_y << "\n";
    g_outgoing_drag_object = drag_object;
}