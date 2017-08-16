#ifndef R64FX_UNDO_REDO_CHAIN_HPP
#define R64FX_UNDO_REDO_CHAIN_HPP

#include <vector>

namespace r64fx{

class UndoRedoItem{
public:
    virtual ~UndoRedoItem() {}

    virtual void undo() = 0;
    virtual void redo() = 0;
};


class UndoRedoChain{
    std::vector<UndoRedoItem*> m_chain;
    int   m_index = -1;

public:
    void addItem(UndoRedoItem *action);

    bool canUndo() const;

    void undo();

    void removeUndone();

    bool canRedo() const;

    void redo();

    void clear();

    int size() const;

    int index() const;
};

}//namespace r64fx

#endif//R64FX_UNDO_REDO_CHAIN_HPP
