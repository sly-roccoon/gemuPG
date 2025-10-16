#pragma once
#include "blockfactory.h"
#include "interface.h"

class Command {
public:
  virtual bool execute() = 0; //? TRUE if cmd should go into history
  virtual void undo() = 0;

protected:
  static block_type_t cur_blocktype_;
  Interface &interface_ = Interface::getInstance();
};

class AddBlockCommand : public Command {
public:
  AddBlockCommand(Vector2f pos) : pos_{floorVec(pos)} {}

  bool execute() override {
    if (pos_.x >= GRID_SIZE / 2 || pos_.y >= GRID_SIZE / 2 ||
        pos_.x < -GRID_SIZE / 2 || pos_.y < -GRID_SIZE / 2)
      return false;

    if (block_) {
      interface_.addBlock(std::move(block_));
      return true;
    }
    return interface_.addBlock(pos_);
  }

  void undo() override {
    block_ = interface_.getBlock(pos_);
    interface_.removeBlock(pos_);
  }

private:
  Vector2f pos_;
  Block *block_ = nullptr;
};

class RemoveBlockCommand : public Command {
public:
  RemoveBlockCommand(Vector2f pos) : pos_{pos}, block_{nullptr} {}
  bool execute() override {
    auto block = interface_.getBlock(floorVec(pos_));
    if (!block)
      return false;

    block_ = block->clone();
    interface_.removeBlock(floorVec(pos_));

    return true;
  }

  ~RemoveBlockCommand() {
    if (block_)
      delete block_;
  }

  void undo() override { interface_.addBlock(std::move(block_)); }

private:
  Vector2f pos_;
  Block *block_;
};

class AddAreaCommand : public Command {
public:
  AddAreaCommand(Vector2f pos) : pos_{floorVec(pos)} {}

  bool execute() override {
    if (pos_.x >= GRID_SIZE / 2 || pos_.y >= GRID_SIZE / 2 ||
        pos_.x < -GRID_SIZE / 2 || pos_.y < -GRID_SIZE / 2)
      return false;

    return interface_.getGrid().addArea(pos_);
  }

  void undo() override { interface_.getGrid().removeArea(pos_); }

private:
  Vector2f pos_;
};

class RemoveAreaCommand : public Command {
public:
  RemoveAreaCommand(Vector2f pos) : pos_{pos} {}

  bool execute() override { return interface_.getGrid().removeArea(pos_); }

  void undo() override { interface_.getGrid().addArea(pos_); }

private:
  Vector2f pos_;
};

//----------------------------
class CommandManager {
public:
  void executeCommand(std::unique_ptr<Command>);
  void undo();
  void redo();

private:
  std::vector<std::unique_ptr<Command>> history;
  std::vector<std::unique_ptr<Command>> redo_stack;
};
