/* To be included in WindowX11.cpp */

namespace{

namespace X11_Atom{
    Atom WM_PROTOCOLS;
    Atom WM_DELETE_WINDOW;
    Atom WM_TRANSIENT_FOR;
    Atom UTF8_STRING;
    Atom TEXT;
    Atom STRING;
    Atom TARGETS;
    Atom MULTIPLE;
    Atom CLIPBOARD;

    Atom _NET_WM_NAME;

    Atom _NET_WM_STATE;
    Atom _NET_WM_STATE_MODAL;

    Atom _NET_WM_WINDOW_TYPE;
    Atom _NET_WM_WINDOW_TYPE_NORMAL;
    Atom _NET_WM_WINDOW_TYPE_DROPDOWN_MENU;
    Atom _NET_WM_WINDOW_TYPE_POPUP_MENU;
    Atom _NET_WM_WINDOW_TYPE_MENU;
    Atom _NET_WM_WINDOW_TYPE_DIALOG;
    Atom _NET_WM_WINDOW_TYPE_TOOLTIP;
    Atom _NET_WM_WINDOW_TYPE_DND;

    Atom _MOTIF_WM_HINTS;

    Atom _R64FX_CLIPBOARD;

    Atom XdndAware;
    Atom XdndEnter;
    Atom XdndLeave;
    Atom XdndTypeList;
    Atom XdndPosition;
    Atom XdndStatus;
    Atom XdndDrop;
    Atom XdndFinished;
    Atom XdndSelection;
    Atom XdndActionCopy;
    Atom XdndActionMove;
    Atom XdndActionLink;
    Atom XdndActionAsk;
    Atom XdndActionPrivate;
}


bool intern_atom(const char* name, Atom &atom, bool only_if_exists)
{
    atom = XInternAtom(g_display, name, only_if_exists);
    if(atom == None)
    {
        cerr << "Failed to intern atom " << name << "\n";
        return false;
    }
    return true;
}


void init_atoms()
{
#define R64FX_INTERN_ATOM(name, only_if_exists) intern_atom(#name, X11_Atom::name, only_if_exists)

        R64FX_INTERN_ATOM( WM_PROTOCOLS,     true  );
        R64FX_INTERN_ATOM( WM_DELETE_WINDOW, true  );
        R64FX_INTERN_ATOM( WM_TRANSIENT_FOR, true  );

        R64FX_INTERN_ATOM( UTF8_STRING,      true  );
        R64FX_INTERN_ATOM( TEXT,             true  );
        R64FX_INTERN_ATOM( STRING,           true  );
        R64FX_INTERN_ATOM( TARGETS,          true  );
        R64FX_INTERN_ATOM( MULTIPLE,         true  );
        R64FX_INTERN_ATOM( CLIPBOARD,        true  );

        R64FX_INTERN_ATOM( _NET_WM_NAME,     true  );
        
        R64FX_INTERN_ATOM( _NET_WM_STATE,        true );
        R64FX_INTERN_ATOM( _NET_WM_STATE_MODAL,  true );

        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE,                true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_NORMAL,         true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_MENU,           true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_POPUP_MENU,     true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_DROPDOWN_MENU,  true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_DIALOG,         true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_TOOLTIP,        true  );
        R64FX_INTERN_ATOM( _NET_WM_WINDOW_TYPE_DND,            true  );

        R64FX_INTERN_ATOM( _MOTIF_WM_HINTS, true );

        R64FX_INTERN_ATOM( _R64FX_CLIPBOARD, false );

        R64FX_INTERN_ATOM( XdndAware,          false );
        R64FX_INTERN_ATOM( XdndEnter,          false );
        R64FX_INTERN_ATOM( XdndLeave,          false );
        R64FX_INTERN_ATOM( XdndTypeList,       false );
        R64FX_INTERN_ATOM( XdndPosition,       false );
        R64FX_INTERN_ATOM( XdndStatus,         false );
        R64FX_INTERN_ATOM( XdndDrop,           false );
        R64FX_INTERN_ATOM( XdndFinished,       false );
        R64FX_INTERN_ATOM( XdndSelection,      false );
        R64FX_INTERN_ATOM( XdndActionCopy,     false );
        R64FX_INTERN_ATOM( XdndActionMove,     false );
        R64FX_INTERN_ATOM( XdndActionLink,     false );
        R64FX_INTERN_ATOM( XdndActionAsk,      false );
        R64FX_INTERN_ATOM( XdndActionPrivate,  false );

#undef R64FX_INTERN_ATOM
}


string atom_name(Atom atom)
{
    char* buff = XGetAtomName(g_display, atom);
    string str(buff);
    XFree(buff);
    return str;
}


map<string, Atom> g_extra_atoms;

Atom get_extra_atom(string name)
{
    auto it = g_extra_atoms.find(name);
    if(it == g_extra_atoms.end())
    {
        Atom atom = XInternAtom(g_display, name.c_str(), False);
        g_extra_atoms[name] = atom;
        return atom;
    }
    else
    {
        return it->second;
    }
}

}//namespace