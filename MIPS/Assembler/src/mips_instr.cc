#include "mips_instr.h"

#include <cassert>

Instr::Instr(int line, OpCode op, RegIdx rs, RegIdx rt, RegIdx rd)
    : line_(line), op_(op), type_(InstrType::R), r1_(rs), r2_(rt), r3_(rd) {}

Instr::Instr(int line, OpCode op, RegIdx rs, RegIdx rd, int imd)
    : line_(line), op_(op), type_(InstrType::S), r1_(rs), r2_(rd) {}

Instr::Instr(int line, OpCode op, Label *label)
    : line_(line), op_(op), type_(InstrType::J), label_(label) {}

Instr::Instr(int line, OpCode op, RegIdx pc, RegIdx seg)
    : line_(line), op_(op), type_(InstrType::JR) {}

Instr::Instr(int line) : line_(line), op_(OpCode::NOP), type_(InstrType::NOP) {}

void Instr::insert_noop(int count) { no_ops_ = std::max(no_ops_, count); }

std::optional<RegIdx> Instr::src_reg1() {
  switch (type_) {
  case InstrType::R:
  case InstrType::S:
  case InstrType::I:
  case InstrType::JR:
    return r1_;
  default:
    return std::nullopt;
  }
}

std::optional<RegIdx> Instr::src_reg2() {
  switch (type_) {
  case InstrType::R:
  case InstrType::JR:
    return r2_;
  case InstrType::I:
    switch (op_) {
    case OpCode::BNEQ:
    case OpCode::BEQ:
    case OpCode::SW:
      return r2_;
    default:
      return std::nullopt;
    }
  default:
    return std::nullopt;
  }
}

std::optional<RegIdx> Instr::dest_reg() {
  switch (type_) {
  case InstrType::R:
    return r3_;
  case InstrType::S:
    return r2_;
  case InstrType::I:
    switch (op_) {
    case OpCode::BNEQ:
    case OpCode::BEQ:
    case OpCode::SW:
      return std::nullopt;
    default:
      return r2_;
    }
  default:
    return std::nullopt;
  }
}

bool Instr::is_jump() {
  switch (type_) {
  case InstrType::J:
  case InstrType::JR:
    return true;
  case InstrType::I:
    switch (op_) {
    case OpCode::BEQ:
    case OpCode::BNEQ:
      return true;
    default:
      return false;
    }
  default:
    return false;
  }
}
