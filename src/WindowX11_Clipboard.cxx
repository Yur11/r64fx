/* To be included in WindowX11.cpp */

namespace{

struct ClipboardImpl{
    Atom                selection;

    ClipboardMetadata   metadata;
    ClipboardMode       mode;
    Atom                requested_clipboard_type;

    void clear()
    {
        metadata.clear();
        requested_clipboard_type = None;
    }
};

ClipboardImpl g_selection;
ClipboardImpl g_clipboard;
ClipboardImpl g_dnd;


void init_clipboard()
{
    g_selection.mode      = ClipboardMode::Selection;
    g_selection.selection = XA_PRIMARY;

    g_clipboard.mode      = ClipboardMode::Clipboard;
    g_clipboard.selection = X11_Atom::CLIPBOARD;

    g_dnd.mode            = ClipboardMode::DragAndDrop;
    g_dnd.selection       = X11_Atom::XdndSelection;
}


ClipboardImpl* clipboard(ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Selection:
            return &g_selection;

        case ClipboardMode::Clipboard:
            return &g_clipboard;

        case ClipboardMode::DragAndDrop:
            return &g_dnd;

        default:
            return nullptr;
    }
}


ClipboardImpl* clipboard(Atom selection)
{
    for(auto cb : {&g_selection, &g_clipboard, &g_dnd})
    {
        if(cb->selection == selection)
            return cb;
    }
    return nullptr;
}

}//namespace


void WindowX11::anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode)
{
    cout << "anounceClipboardData: " << mode << "\n";

    auto cb = clipboard(mode);
    if(!cb)
    {
        cerr << "Bad clipboard mode!\n";
        return;
    }

    XSetSelectionOwner(g_display, cb->selection, m_xwindow, CurrentTime);
    cb->metadata = metadata;
}


void WindowX11::requestClipboardData(ClipboardDataType type, ClipboardMode mode)
{
    cout << "requestClipboardData: " << mode << "\n";

    auto cb = clipboard(mode);
    if(!cb)
    {
        cerr << "Bad clipboard mode!\n";
        return;
    }

    Atom type_atom = get_extra_atom(type.name());

    XConvertSelection(
        g_display,
        cb->selection,
        type_atom,
        X11_Atom::_R64FX_CLIPBOARD,
        m_xwindow,
        CurrentTime
    );

    cb->requested_clipboard_type = type_atom;
}


void WindowX11::requestClipboardMetadata(ClipboardMode mode)
{
    auto cb = clipboard(mode);
    if(!cb)
    {
        cerr << "Bad clipboard mode!\n";
        return;
    }

    XConvertSelection(
        g_display,
        cb->selection,
        X11_Atom::TARGETS,
        X11_Atom::_R64FX_CLIPBOARD,
        m_xwindow,
        CurrentTime
    );
}


void WindowX11::selectionClearEvent()
{
    XSelectionClearEvent &in = g_incoming_event->xselectionclear;

    auto cb = clipboard(in.selection);
    if(cb)
    {
        cb->clear();
    }
}


void WindowX11::selectionRequestEvent(WindowEventDispatcherIface* events)
{
    XSelectionRequestEvent  &in   = g_incoming_event->xselectionrequest;

    XEvent xevent;
    g_outgoing_event = &xevent;
    XSelectionEvent &out = xevent.xselection;

    if(in.property == None)
    {
        cout << "property None\n";
        return;
    }

    auto cb = clipboard(in.selection);
    if(!cb)
    {
        cerr << "Bad clipboard mode!\n";
        return;
    }

    out.type      = SelectionNotify;
    out.display   = in.display;
    out.requestor = in.requestor;
    out.selection = in.selection;
    out.target    = in.target;
    out.property  = in.property;
    out.time      = in.time;

    if(in.target == X11_Atom::TARGETS)
    {
        vector<Atom> targets = {X11_Atom::TARGETS};
        for(auto &type : cb->metadata)
        {
            string type_name(type.name());
            targets.push_back(get_extra_atom(type_name));
        }

        XChangeProperty(
            g_display,
            in.requestor,
            in.property,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*) targets.data(),
            targets.size()
        );

        if(!XSendEvent(g_display, in.requestor, False, NoEventMask, g_outgoing_event))
        {
            cerr << "Failed to send selection event!\n";
        }
    }
    else
    {
        ClipboardDataType cdt(atom_name(in.target));

        if(cdt.isGood())
        {
            events->clipboardDataTransmitEvent(
                this,
                cdt,
                cb->mode,
                [](Window* window, void* data, int size)
                {
                    auto window_x11 = (WindowX11*) window;
                    window_x11->transmitRequestedSelection(data, size);
                }
            );
        }
    }

    g_outgoing_event = nullptr;
}


void WindowX11::transmitRequestedSelection(void* data, int size)
{
    XSelectionRequestEvent &in = g_incoming_event->xselectionrequest;

    if(data != nullptr && size > 0)
    {
        string str((const char*)data, size);

        XChangeProperty(
            g_display,
            in.requestor,
            in.property,
            in.target,
            8,
            PropModeReplace,
            (unsigned char*) data,
            size
        );

        if(!XSendEvent(g_display, in.requestor, False, NoEventMask, g_outgoing_event))
        {
            cerr << "Failed to send selection event!\n";
        }
    }
}


void WindowX11::selectionNotifyEvent(WindowEventDispatcherIface* events)
{
    XSelectionEvent &in = g_incoming_event->xselection;

    auto cb = clipboard(in.selection);
    if(!cb)
    {
        cerr << "Bad clipboard mode!\n";
        return;
    }

    if(in.property == None) //Owner could not convert selection.
        return;

    if(in.target == X11_Atom::TARGETS)
    {
        unsigned long  nitems = 0;
        unsigned char* data   = nullptr;
        int format = 0;

        get_window_property(
            m_xwindow,
            in.property,
            false,
            nitems,
            data,
            format
        );


        if(format == 32 && nitems > 0)
        {
            ClipboardMetadata metadata;
            auto atoms = (Atom*)data;
            for(int i=0; i<(int)nitems; i++)
            {
                Atom atom = atoms[i];
                string name = atom_name(atom);
                metadata.push_back(name);
            }

            events->clipboardMetadataRecieveEvent(
                this, metadata, cb->mode
            );
        }
    }
    else if(in.target == cb->requested_clipboard_type)
    {
        unsigned long  nitems = 0;
        unsigned char* data   = nullptr;
        int format = 0;

        get_window_property(
            m_xwindow,
            in.property,
            false,
            nitems,
            data,
            format
        );

        ClipboardDataType cdt;
        if(in.target == X11_Atom::UTF8_STRING || in.target == X11_Atom::TEXT)
        {
            cdt = "text/plain";
        }
        else
        {
            cdt = atom_name(in.target);
        }

        events->clipboardDataRecieveEvent(
            this,
            cdt,
            cb->mode,
            (void*)data,
            (int)nitems
        );
    }
}
