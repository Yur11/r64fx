/* To be included in WindowX11.cpp */

namespace{

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
    g_dnd_metadata.clear();

    const XClientMessageEvent &in = g_incoming_event->xclient;
    const long int* dnd_enter_data = in.data.l;

    ::Window dnd_source_window = dnd_enter_data[0];
    if(dnd_source_window == None)
        return;

    if(dnd_enter_data[1] & 1)
    {
        Atom type;
        int format = 0;
        unsigned long nitems = 0;
        unsigned long bytes_after = 0;
        unsigned char* data = nullptr;

        auto result = XGetWindowProperty(
            g_display,
            dnd_source_window,
            X11_Atom::XdndTypeList,
            0, 0, False,
            XA_ATOM, &type, &format,
            &nitems, &bytes_after,
            &data
        );

        if(result == Success)
        {
            unsigned int items_to_read = (bytes_after >> 2);
            if(items_to_read)
            {
                format = 0;
                nitems = 0;
                bytes_after = 0;
                data = nullptr;

                auto result = XGetWindowProperty(
                    g_display,
                    dnd_source_window,
                    X11_Atom::XdndTypeList,
                    0, items_to_read, False,
                    XA_ATOM, &type, &format,
                    &nitems, &bytes_after,
                    &data
                );

                if(result == Success && nitems > 0)
                {
                    if(type == XA_ATOM && format == 32 && nitems == items_to_read)
                    {
                        Atom* atoms = (Atom*) data;
                        for(unsigned int i=0; i<nitems; i++)
                        {
                            string name = atom_name(atoms[i]);
                            g_dnd_metadata.push_back(name);
                        }
                    }
                    XFree(data);
                }
            }
        }
    }
    else
    {
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


void WindowX11::xdndLeaveEvent()
{
    g_incoming_drag = false;
    g_events->dndLeaveEvent(this);
}


void WindowX11::xdndPositionEvent()
{
    const XClientMessageEvent &in = g_incoming_event->xclient;
    const long int* dnd_postion_data = in.data.l;

    ::Window drag_source = (::Window) dnd_postion_data[0];
    ::Window drag_target = m_xwindow;

    int x = (dnd_postion_data[2] >> 16) & 0xFFFF;
    int y = dnd_postion_data[2] & 0xFFFF;

    Atom action = dnd_postion_data[4];

    g_incoming_drag = true;
    updateAttrs();
    bool accept = false;
    g_events->dndMoveEvent(this, x - this->x(), y - this->y(), g_dnd_metadata, accept);

    XEvent out_xevent;
    XClientMessageEvent &out = out_xevent.xclient;
    long int* dnd_status_data = out.data.l;
    out.type           = ClientMessage;
    out.display        = g_display;
    out.window         = drag_source;
    out.format         = 32;
    out.message_type   = X11_Atom::XdndStatus;
    dnd_status_data[0] = drag_target;
    dnd_status_data[1] = (accept ? 1 : 0);
    dnd_status_data[2] = 0;
    dnd_status_data[3] = 0;
    dnd_status_data[4] = (accept ? action : None);

    if(!XSendEvent(g_display, drag_source, False, NoEventMask, &out_xevent))
    {
        cerr << "Failed to send XdndStatus event!\n";
    }
}


void WindowX11::xdndDropEvent()
{
    const XClientMessageEvent &in = g_incoming_event->xclient;
    const long int* dnd_drop_data = in.data.l;

    g_incoming_drop_source = (::Window) dnd_drop_data[0];
    g_incoming_drop_target = m_xwindow;

    ClipboardDataType data_type;
    bool accepted = false;
    g_events->dndDropEvent(this, g_dnd_metadata, data_type, accepted);

    if(accepted)
    {
        cout << "accepted: " << data_type.name() << "\n";
        requestClipboardData(data_type, ClipboardMode::DragAndDrop);
    }
}


void WindowX11::sendDndFinished()
{
    cout << "sendDndFinished()\n";

    XEvent out_xevent;
    XClientMessageEvent &out = out_xevent.xclient;
    long int* dnd_status_data = out.data.l;
    out.type           = ClientMessage;
    out.display        = g_display;
    out.window         = g_incoming_drop_source;
    out.format         = 32;
    out.message_type   = X11_Atom::XdndFinished;
    dnd_status_data[0] = g_incoming_drop_target;
    dnd_status_data[1] = 0;
    dnd_status_data[2] = 0;
    dnd_status_data[3] = 0;
    dnd_status_data[4] = 0;

    if(!XSendEvent(g_display, g_incoming_drop_source, False, NoEventMask, &out_xevent))
    {
        cerr << "Failed to send XdndStatus event!\n";
    }

    g_incoming_drag = false;
    g_incoming_drop_source = None;
    g_incoming_drop_target = None;
}