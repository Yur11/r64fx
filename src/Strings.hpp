#ifndef R64FX_STRINGS_HPP
#define R64FX_STRINGS_HPP

namespace r64fx{

/* Translation string keys */
enum class Strings{
    Session,  NewSession, OpenSession, SaveSession, SaveSessionAs, Quit,
    Project,  NewProject, OpenProject, SaveProject, SaveProjectAs, CreatePlayer, CloseProject,
    Edit,     Cut, Copy, Paste, Undo, Redo,
    View,     NoView,
    Help,     NoHelp,

    StringCount
};

}//namespace r64fx

#endif//R64FX_STRINGS_HPP
