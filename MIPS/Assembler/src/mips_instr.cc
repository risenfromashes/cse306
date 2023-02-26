#include "mips_instr.h"

#include <algorithm>
#include <cassert>

Instr::Instr(int line, OpCode op, RegIdx rs, RegIdx rt, RegIdx rd)
    : line_(line), op_(op), type_(InstrType::R), r1_(rs), r2_(rt), r3_(rd) {}

Instr::Instr(int line, OpCode op, RegIdx rs, RegIdx rd, int imd)
    : line_(line), op_(op), type_(InstrType::I), r1_(rs), r2_(rd), imd_(imd) {}

Instr::Instr(int line, OpCode op, RegIdx rs, RegIdx rd, Label *label)
    : line_(line), op_(op), type_(InstrType::JC), r1_(rs), r2_(rd),
      label_(label) {}

Instr::Instr(int line, OpCode op, Label *label)
    : line_(line), op_(op), type_(InstrType::J), label_(label) {}

Instr::Instr(int line, OpCode op, RegIdx pc, RegIdx seg)
    : line_(line), op_(op), type_(InstrType::JR) {}

Instr::Instr(int line) : line_(line), op_(OpCode::NOP), type_(InstrType::NOP) {}

void Instr::insert_noop(int count) { no_ops_ = std::max(no_ops_, count); }

std::optional<RegIdx> Instr::src_reg1() {
  switch (type_) {
  case InstrType::R:
  case InstrType::I:
    return r2_;
  case InstrType::JC:
  case InstrType::JR:
    return r1_;
  default:
    return std::nullopt;
  }
}

std::optional<RegIdx> Instr::src_reg2() {
  switch (type_) {
  case InstrType::R:
    return r3_;
  case InstrType::JR:
  case InstrType::JC:
    return r2_;
  case InstrType::I:
    switch (op_) {
    case OpCode::SW:
      return r1_;
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
    return r1_;
  case InstrType::I:
    switch (op_) {
    case OpCode::SW:
      return std::nullopt;
    default:
      return r1_;
    }
  default:
    return std::nullopt;
  }
}

bool Instr::is_jump() {
  switch (type_) {
  case InstrType::J:
  case InstrType::JC:
  case InstrType::JR:
    return true;
  default:
    return false;
  }
}

std::string_view to_string(OpCode op) {
  switch (op) {
  case OpCode::NOR:
    return "nor";
  case OpCode::SRL:
    return "srl";
  case OpCode::AND:
    return "and";
  case OpCode::SLL:
    return "sll";
  case OpCode::BNEQ:
    return "bneq";
  case OpCode::J:
    return "j";
  case OpCode::ANDI:
    return "andi";
  case OpCode::SUBI:
    return "subi";
  case OpCode::LW:
    return "lw";
  case OpCode::SW:
    return "sw";
  case OpCode::SUB:
    return "sub";
  case OpCode::ADDI:
    return "addi";
  case OpCode::OR:
    return "or";
  case OpCode::BEQ:
    return "beq";
  case OpCode::ORI:
    return "ori";
  case OpCode::ADD:
    return "add";
  case OpCode::JR:
    return "jr";
  case OpCode::NOP:
    return "nop";
  }
  return "";
}

std::string_view to_string(RegIdx reg) {
  switch (reg) {
  case RegIdx::ZERO:
    return "$zero";
  case RegIdx::T0:
    return "$t0";
  case RegIdx::T1:
    return "$t1";
  case RegIdx::T2:
    return "$t2";
  case RegIdx::T3:
    return "$t3";
  case RegIdx::T4:
    return "$t4";
  case RegIdx::T5:
    return "$t5";
  case RegIdx::T6:
    return "$t6";
  case RegIdx::T7:
    return "$t7";
  case RegIdx::S0:
    return "$s0";
  case RegIdx::S1:
    return "$s1";
  case RegIdx::S2:
    return "$s2";
  case RegIdx::S3:
    return "$s3";
  case RegIdx::IO:
    return "$io";
  case RegIdx::BP:
    return "$bp";
  case RegIdx::SP:
    return "$sp";
  }
  return "";
}

template <int N> void print_hex(std::string &vec, int64_t v) {
  char ch;
  for (int i = 0; i < N; i++) {
    uint8_t l = (v >> i * 4) & 0xF;
    if (l < 10) {
      ch = '0' + l;
    } else {
      ch = 'A' + (l - 10);
    }
    vec.push_back(ch);
  }
}

// op jl j addr
// addr   jh jl op

std::string Instr::to_hex_str(bool c_pref) {
  uint8_t op = (uint8_t)op_;
  std::string line;
  print_hex<1>(line, op);
  using enum InstrType;
  switch (type_) {
  case R:
    print_hex<1>(line, (uint8_t)*r1_);
    print_hex<1>(line, (uint8_t)*r2_);
    print_hex<1>(line, (uint8_t)*r3_);
    break;
  case I:
    print_hex<1>(line, (uint8_t)*r1_);
    print_hex<1>(line, (uint8_t)*r2_);
    print_hex<1>(line, (uint8_t)*imd_);
    break;
  case JC:
    print_hex<1>(line, (uint8_t)*r1_);
    print_hex<1>(line, (uint8_t)*r2_);
    print_hex<1>(line, (uint8_t)(*label_)->offset(line_));
    break;
  case J:
    print_hex<2>(line, (uint8_t)(*label_)->address());
    print_hex<1>(line, 0b0000u);
    break;
  case JR:
    print_hex<1>(line, (uint8_t)*r1_);
    print_hex<1>(line, (uint8_t)*r2_);
    print_hex<1>(line, (uint8_t)*r3_);
    print_hex<1>(line, 0b0100u);
    break;
  case NOP:
    print_hex<2>(line, 0);
    print_hex<1>(line, 0b1000u);
    break;
  }
  std::reverse(line.begin(), line.end());
  std::string ret;
  if (!c_pref) {
    for (int i = 0; i < line.size(); i++) {
      if (i && i % 2 == 0) {
        ret += " ";
      }
      ret += line[i];
    }
  } else {
    for (int i = 0; i < line.size(); i++) {
      if (i % 4 == 0) {
        ret += "0x";
      }
      ret += line[i];
      if (i % 4 == 3) {
        ret += ", ";
      }
    }
  }
  return ret;
}

std::string Instr::to_string() {
  std::string line;
  line += ::to_string(op_);
  line += " ";
  using enum InstrType;
  switch (type_) {
  case R:
    line += ::to_string(*r1_);
    line += ", ";
    line += ::to_string(*r2_);
    line += ", ";
    line += ::to_string(*r3_);
    break;
  case I:
    line += ::to_string(*r1_);
    line += ", ";
    line += ::to_string(*r2_);
    line += ", ";
    line += std::to_string(*imd_);
    break;
  case JC:
    line += ::to_string(*r1_);
    line += ", ";
    line += ::to_string(*r2_);
    line += ", ";
    line += (*label_)->name();
    break;
  case J:
    line += (*label_)->name();
    break;
  case JR:
    line += ::to_string(*r1_);
    line += ", ";
    line += ::to_string(*r2_);
    break;
  case NOP:
    break;
  }
  // line += "(" + std::to_string(no_ops_) + ")";
  return line;
}