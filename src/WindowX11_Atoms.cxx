/* To be included in WindowX11.cpp */

namespace{

namespace X11_Atom{
    Atom WM_PROTOCOLS;
    Atom WM_DELETE_WINDOW;
    Atom _NET_WM_NAME;
    Atom UTF8_STRING;
    Atom TEXT;
    Atom TARGETS;
    Atom MULTIPLE;
    Atom _R64FX_SELECTION;
    Atom CLIPBOARD;
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
        R64FX_INTERN_ATOM( _NET_WM_NAME,     true  );
        R64FX_INTERN_ATOM( UTF8_STRING,      true  );
        R64FX_INTERN_ATOM( TEXT,             true  );
        R64FX_INTERN_ATOM( TARGETS,          true  );
        R64FX_INTERN_ATOM( MULTIPLE,         true  );
        R64FX_INTERN_ATOM( _R64FX_SELECTION, false );
        R64FX_INTERN_ATOM( CLIPBOARD,        true  );
        R64FX_INTERN_ATOM( _R64FX_CLIPBOARD, false );

        R64FX_INTERN_ATOM( XdndAware,     false );
        R64FX_INTERN_ATOM( XdndEnter,     false );
        R64FX_INTERN_ATOM( XdndLeave,     false );
        R64FX_INTERN_ATOM( XdndTypeList,  false );
        R64FX_INTERN_ATOM( XdndPosition,  false );
        R64FX_INTERN_ATOM( XdndStatus,    false );
        R64FX_INTERN_ATOM( XdndDrop,      false );
        R64FX_INTERN_ATOM( XdndFinished,  false );
        R64FX_INTERN_ATOM( XdndSelection, false );

#undef R64FX_INTERN_ATOM
}


string atom_name(Atom atom)
{
    char* buff = XGetAtomName(g_display, atom);
    string str(buff);
    XFree(buff);
    return str;
}

}//namespace