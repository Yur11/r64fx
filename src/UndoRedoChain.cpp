#include "UndoRedoChain.hpp"

namespace r64fx{

void UndoRedoChain::setData(void* data)
{
    m_data = data;
}


void* UndoRedoChain::data() const
{
    return m_data;
}


void UndoRedoChain::addItem(UndoRedoItem *action)
{
    if(!m_chain.empty() && m_index < (int)m_chain.size())
    {
        m_chain.erase(m_chain.begin() + m_index, m_chain.end());
    }
    m_chain.push_back(action);
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
        act->undo(m_data);
        m_index--;
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
        act->redo(m_data);
    }
}


void UndoRedoChain::clear()
{
    m_chain.clear();
}


int UndoRedoChain::size() const
{
    return m_chain.size();
}

}//namespace r64fx