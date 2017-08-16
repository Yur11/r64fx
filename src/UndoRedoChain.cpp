#include "UndoRedoChain.hpp"

namespace r64fx{

void UndoRedoChain::addItem(UndoRedoItem *action)
{
    removeUndone();
    m_chain.push_back(action);
    m_index = m_chain.size() - 1;
}


bool UndoRedoChain::canUndo() const
{
    return m_index >= 0;
}


void UndoRedoChain::undo()
{
    if(m_chain.empty())
        return;

    if(canUndo())
    {
        auto &act = m_chain[m_index];
        act->undo();
        m_index--;
    }
}


void UndoRedoChain::removeUndone()
{
    if(!m_chain.empty() && (m_index + 1) < (int)m_chain.size())
    {
        for(auto it = (m_chain.begin() + m_index + 1); it != m_chain.end(); it++)
        {
            delete *it;
        }
        m_chain.erase(m_chain.begin() + m_index + 1, m_chain.end());
    }
}


bool UndoRedoChain::canRedo() const
{
    return (m_index + 1) < (int)m_chain.size();
}


void UndoRedoChain::redo()
{
    if(m_chain.empty())
        return;

    if(canRedo())
    {
        m_index++;
        auto &act = m_chain[m_index];
        act->redo();
    }
}


void UndoRedoChain::clear()
{
    for(auto item : m_chain)
    {
        delete item;
    }
    m_chain.clear();
    m_index = -1;
}


int UndoRedoChain::size() const
{
    return m_chain.size();
}


int UndoRedoChain::index() const
{
    return m_index;
}

}//namespace r64fx
