#include "token.h"
#include <cassert>

Token::Token(OpCode op) : data_(op) {}
Token::Token(RegIdx reg) : data_(reg) {}
Token::Token(int imd) : data_(imd) {}
Token::Token(Label *label) : data_(label) {}

bool Token::is_opcode() { return std::holds_alternative<OpCode>(data_); }
bool Token::is_reg() { return std::holds_alternative<RegIdx>(data_); }
bool Token::is_imd() { return std::holds_alternative<int>(data_); }
bool Token::is_label() { return std::holds_alternative<Label *>(data_); }

OpCode Token::opcode() {
  assert(is_opcode());
  return std::get<OpCode>(data_);
}

RegIdx Token::reg() {
  assert(is_reg());
  return std::get<RegIdx>(data_);
}

int Token::imd() {
  assert(is_imd());
  return std::get<int>(data_);
}

Label *Token::label() {
  assert(is_label());
  return std::get<Label *>(data_);
}
