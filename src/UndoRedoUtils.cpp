#include "UndoRedoUtils.hpp"
#include "KeyboardModifiers.hpp"

namespace r64fx{

bool undo_redo(UndoRedoChain* uc, int key)
{
    if(Keyboard::CtrlDown() && Keyboard::ShiftDown() && key == Keyboard::Key::Z)
    {
        uc->redo();
        return true;
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::Z)
    {
        uc->undo();
        return true;
    }
    else if(Keyboard::CtrlDown() && key == Keyboard::Key::Y)
    {
        uc->redo();
        return true;
    }
    return false;
}

}//namespace r64
