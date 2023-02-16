#include "assembler_context.h"

#include <cassert>

void AssemblerContext::add_token(Token token) {
  current_line_.push_back(std::move(token));
}

void AssemblerContext::new_line(int line) {
  if (current_line_.empty()) {
    return;
  }
  auto op = current_line_[0];
  assert(op.is_opcode());
  switch (op.opcode()) {
    /* r type */
  case OpCode::NOR:
  case OpCode::AND:
  case OpCode::SUB:
  case OpCode::ADD:
  case OpCode::OR:
    assert(current_line_.size() == 4);
    assert(current_line_[1].is_reg());
    assert(current_line_[2].is_reg());
    assert(current_line_[3].is_reg());
    add_instr(Instr(line, op.opcode(), current_line_[1].reg(),
                    current_line_[2].reg(), current_line_[3].reg()));
    return;
    /* s type */
  case OpCode::SRL:
  case OpCode::SLL:
    /* i type */
  case OpCode::ADDI:
  case OpCode::ANDI:
  case OpCode::SUBI:
  case OpCode::ORI:
  case OpCode::BNEQ:
  case OpCode::BEQ:
  case OpCode::LW:
  case OpCode::SW:
    /* j type */
  case OpCode::J:
    /* jr type */
  case OpCode::JR:
    /* no-op */
  case OpCode::NOP:
    break;
  }
}
