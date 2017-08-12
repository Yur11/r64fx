#ifndef R64FX_UNDO_REDO_UTILS_HPP
#define R64FX_UNDO_REDO_UTILS_HPP

#include "UndoRedoChain.hpp"
#include "Keyboard.hpp"

namespace r64fx{

bool undo_redo(UndoRedoChain* uc, int key);

}//namespace r64fx

#endif//R64FX_UNDO_REDO_UTILS_HPP
