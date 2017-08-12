#ifndef R64FX_TEXT_EDITING_UTILS_HPP
#define R64FX_TEXT_EDITING_UTILS_HPP

#include "TextPainter.hpp"
#include "UndoRedoChain.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"


namespace r64fx{

bool cursor_hori(TextPainter* tp, int key, bool* touched_selection);

bool cursor_vert(TextPainter* tp, int key, bool* touched_selection);

bool select_all(TextPainter* tp, int key, bool* touched_selection);

void delete_text_at_cursor(TextPainter* tp, UndoRedoChain* uc, bool backspace);

bool delete_text(TextPainter* tp, int key, UndoRedoChain* uc, bool* touched_selection);

bool insert_text(TextPainter* tp, UndoRedoChain* uc, const std::string &text);

}//namespace r64fx

#endif//R64FX_TEXT_EDITING_UTILS_HPP
