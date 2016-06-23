/* To be included in WindowX11.cpp */

namespace{

::Window dnd_source(const long* msg_data)
{
    return (::Window) msg_data[0];
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


int find_xdnd_version(::Window window)
{
    Atom actual_type           = None;
    int actual_format          = 0;
    unsigned long nitems       = 0;
    unsigned long bytes_after  = 0;
    unsigned char* prop_data   = nullptr;

    XGetWindowProperty(
        g_display,
        window,
        X11_Atom::XdndAware,
        0, 1, False,
        AnyPropertyType,
        &actual_type,
        &actual_format,
        &nitems,
        &bytes_after,
        &prop_data
    );

    int xdnd_version = -1;

    if(prop_data)
    {
        if(actual_format == 32 && actual_type == XA_ATOM && nitems == 1)
        {
            Atom* ptr = (Atom*) prop_data;
            xdnd_version = ptr[0];
        }
        XFree(prop_data);
    }

    return xdnd_version;
}


void find_dnd_target(::Window parent, int x, int y, ::Window* out_target, int* out_xdnd_version)
{
    ::Window  root_stub    = None;
    ::Window  parent_stub  = None;
    ::Window* children  = nullptr;
    unsigned int children_count = 0;

    if(XQueryTree(
        g_display, parent,
        &root_stub, &parent_stub,
        &children, &children_count
    ))
    {
        for(int i=(children_count - 1); i>=0; i--)
        {
            ::Window window = children[i];

            if(g_outgoing_drag_object && window == g_outgoing_drag_object->xWindow())
            {
                continue;
            }

            XWindowAttributes attrs;
            g_ignore_bad_window = true;
            Status status = XGetWindowAttributes(g_display, window, &attrs);
            g_ignore_bad_window = false;
            if(!status)
            {
                continue;
            }

            if((x >= attrs.x) && (x < (attrs.x + attrs.width)) && (y >= attrs.y) && (y < (attrs.y + attrs.height)))
            {
                if(attrs.map_state == IsViewable)
                {
                    int window_xdnd_version = find_xdnd_version(window);
                    if(window_xdnd_version >= 0)
                    {
                        out_target[0] = window;
                        out_xdnd_version[0] = window_xdnd_version;
                        return;
                    }

                    ::Window child = None;
                    int child_xdnd_version = -1;
                    find_dnd_target(window, x - attrs.x, y - attrs.y, &child, &child_xdnd_version);
                    if(child != None)
                    {
                        out_target[0] = child;
                        out_xdnd_version[0] = child_xdnd_version;
                        return;
                    }
                }
            }
        }//for every child
    }

    out_xdnd_version[0] = -1;
}

}//namespace


void WindowX11::setupDnd()
{
    Atom dnd_version = 5;
    XChangeProperty(
        g_display,
        m_xwindow,
        X11_Atom::XdndAware,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&dnd_version,
        1
    );
}


void WindowX11::startDrag(Window* drag_object, int anchor_x, int anchor_y)
{
    if(!drag_object || g_outgoing_drag_object)
        return;

    g_outgoing_drag_object = dynamic_cast<WindowX11*>(drag_object);
    if(!g_outgoing_drag_object)
        return;

    g_drag_anchor_x = anchor_x;
    g_drag_anchor_y = anchor_y;
    grabMouse();
}


void WindowX11::dndMove(int eventx, int eventy)
{
    cout << "dndMove: " << eventx << ", " << eventy << "\n";

    int screen_x = eventx + x();
    int screen_y = eventy + y();
    int dnd_obj_x = screen_x - g_drag_anchor_x;
    int dnd_obj_y = screen_y - g_drag_anchor_y;
    g_outgoing_drag_object->setPosition({dnd_obj_x, dnd_obj_y});

    ::Window  root      = None;
    ::Window  parent    = None;
    ::Window* children  = nullptr;
    unsigned int children_count = 0;

    if(XQueryTree(
        g_display, xWindow(),
        &root, &parent,
        &children, &children_count
    ))
    {
        ::Window xdnd_target = None;
        int target_xdnd_version = -1;

        find_dnd_target(root, screen_x, screen_y, &xdnd_target, &target_xdnd_version);

        Window* target_window = nullptr;
        if(xdnd_target)
        {
            target_window = getWindowFromXWindow(xdnd_target);
        }

        cout << xdnd_target << " -> " << target_xdnd_version << " --> " << target_window << "\n";
    }
}


void WindowX11::xdndEnterEvent()
{
    cout << "WindowX11::xdndEnterEvent()\n";

    const XClientMessageEvent &in = g_incoming_event->xclient;
    const long int* dnd_enter_data = in.data.l;

    if(dnd_enter_data[1] & 1)
    {
        cout << "More types!\n";
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
    }
    else
    {
        cout << "Less types!\n";

        g_dnd_metadata.clear();
        for(int i=2; i<5; i++)
        {
            if(dnd_enter_data[i] != None)
            {
                string name = atom_name(dnd_enter_data[i]);
                g_dnd_metadata.push_back(name);
            }
        }
    }
}


void WindowX11::xdndDropEvent()
{
    const XClientMessageEvent &in = g_incoming_event->xclient;

    ::Window drag_target = m_xwindow;
    ::Window drag_source = dnd_source(in.data.l);

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

    g_events->dndDropEvent(this);

    g_incoming_drag = false;
}