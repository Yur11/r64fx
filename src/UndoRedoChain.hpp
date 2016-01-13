#ifndef R64FX_UNDO_REDO_CHAIN_HPP
#define R64FX_UNDO_REDO_CHAIN_HPP

#include <vector>

namespace r64fx{

class UndoRedoItem{
public:
    virtual void undo(void* data) = 0;
    virtual void redo(void* data) = 0;
};


class UndoRedoChain{
    std::vector<UndoRedoItem*> m_chain;
    int   m_index = -1;
    void* m_data  = nullptr;

public:
    void setData(void* data);

    void* data() const;

    void addItem(UndoRedoItem *action);

    bool canUndo() const;

    void undo();

    bool canRedo() const;

    void redo();

    void clear();

    int size() const;

};

}//namespace r64fx

#endif//R64FX_UNDO_REDO_CHAIN_HPP