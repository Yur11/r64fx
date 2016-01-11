#ifndef R64FX_UNDO_REDO_CHAIN_HPP
#define R64FX_UNDO_REDO_CHAIN_HPP

#include <vector>

namespace r64fx{

struct UndoRedoAction{
    void (*undo)(void* action_data, void* chain_data) = nullptr;
    void (*redo)(void* action_data, void* chain_data) = nullptr;
    void* data = nullptr;
};


class UndoRedoChain{
    std::vector<UndoRedoAction> m_chain;
    int   m_index = -1;
    void* m_data  = nullptr;

public:
    void setData(void* data);

    void* data() const;

    void addAction(const UndoRedoAction &action);

    void addAction(
        void (*undo)(void* action_data, void* chain_data),
        void (*redo)(void* action_data, void* chain_data),
        void* data
    );

    bool canUndo() const;

    void undo();

    bool canRedo() const;

    void redo();

    void doLatest();

    void clear();

    int size() const;

};

}//namespace r64fx

#endif//R64FX_UNDO_REDO_CHAIN_HPP