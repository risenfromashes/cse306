#pragma once

#include "mips_instr.h"

#include <optional>
#include <vector>

class Block {
public:
  Block();
  Block(Label *label);

  Label *label() { return label_; }

  void add_instr(Instr instr);
  std::vector<Instr> out();

private:
  void find_nexts();

  Label *label_;
  std::vector<Instr> instrs_;
};
