#pragma once
#include "interface.h"
#include "blockfactory.h"

class Command
{
public:
    virtual bool execute() = 0; //? TRUE if cmd should go into history
    virtual void undo() = 0;

protected:
    static blockType cur_blocktype_;
    Interface &interface_ = Interface::getInstance();
};

class AddBlockCommand : public Command
{
public:
    AddBlockCommand(Vector2 pos) : pos_{pos} {}

    bool execute()
    {
        return interface_.addBlock(cur_blocktype_, floorVec(pos_));
    }

    void undo()
    {
        interface_.removeBlock(floorVec(pos_));
    }

private:
    Vector2 pos_;
};

class RemoveBlockCommand : public Command
{
public:
    RemoveBlockCommand(Vector2 pos) : pos_{pos} {}
    bool execute()
    {
        auto block = interface_.getBlock(floorVec(pos_));
        if (!block)
            return false;

        block_ = block->clone();
        interface_.removeBlock(floorVec(pos_));

        return true;
    }

    void undo()
    {
        interface_.addBlock(block_->clone()); // TODO: smart pointers
    }

private:
    Vector2 pos_;
    std::shared_ptr<Block> block_;
};

//----------------------------
class CommandManager
{
public:
    void executeCommand(std::unique_ptr<Command>);
    void undo();
    void redo();

private:
    std::vector<std::unique_ptr<Command>> history;
    std::vector<std::unique_ptr<Command>> redo_stack;
};
