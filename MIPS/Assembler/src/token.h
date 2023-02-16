#pragma once

#include "mips_instr.h"

class Token {
public:
  Token(OpCode);
  Token(RegIdx);
  Token(int);
  Token(Label *);

  bool is_opcode();
  bool is_reg();
  bool is_imd();
  bool is_label();

  OpCode opcode();
  RegIdx reg();
  int imd();
  Label *label();

private:
  std::variant<OpCode, RegIdx, int, Label *> data_;
};
