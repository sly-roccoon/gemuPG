#include "command.h"

block_type_t Command::cur_blocktype_ = BLOCK_GENERATOR;

//--------------------------
void CommandManager::executeCommand (std::unique_ptr<Command> cmd)
{
    if (! cmd->execute())
        return;
    history.push_back (std::move (cmd));
    redo_stack.clear();
}

void CommandManager::undo()
{
    if (! history.empty())
    {
        auto cmd = std::move (history.back());
        history.pop_back();
        cmd->undo();
        redo_stack.push_back (std::move (cmd));
    }
}

void CommandManager::redo()
{
    if (! redo_stack.empty())
    {
        auto cmd = std::move (redo_stack.back());
        redo_stack.pop_back();
        cmd->execute();
        history.push_back (std::move (cmd));
    }
}
