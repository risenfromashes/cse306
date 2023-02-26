#pragma once

#include <iostream>
#include <optional>
#include <variant>

enum class OpCode : uint8_t {
  NOR = 0b0000,
  SRL = 0b0001,
  AND = 0b0010,
  SLL = 0b0011,
  BNEQ = 0b0100,
  J = 0b0101,
  ANDI = 0b0110,
  SUBI = 0b0111,
  LW = 0b1000,
  SW = 0b1001,
  SUB = 0b1010,
  ADDI = 0b1011,
  OR = 0b1100,
  BEQ = 0b1101,
  ORI = 0b1110,
  ADD = 0b1111,
  /* special instructions with 2 higher bits */
  JR = 0b010101,
  NOP = 0b100101
};

std::string_view to_string(OpCode op);

constexpr int N_REGISTER = 16;
enum class RegIdx : int {
  ZERO,
  T0,
  T1,
  T2,
  T3,
  T4,
  T5,
  T6,
  T7,
  S0,
  S1,
  S2,
  S3,
  IO,
  BP,
  SP
};

std::string_view to_string(RegIdx reg);

enum class InstrType { R, I, JC, J, JR, NOP };

class Label {
public:
  Label(std::string name) : name_(name), id_(last_id_++) {}
  int id() { return id_; }

  void set_address(int address) { address_ = address; }
  int address() { return address_; }
  int offset(int pc) { return address_ - (pc + 1); }

  std::string_view name() { return name_; }

private:
  int address_ = 0;
  int id_;
  std::string name_;
  static inline int last_id_ = 0;
};

class Instr {
public:
  static Instr noop() { return Instr(0); }

  Instr(int line);
  Instr(int line, OpCode op, RegIdx rs, RegIdx rt, RegIdx rd);
  Instr(int line, OpCode op, RegIdx rs, RegIdx rt, int imd);
  Instr(int line, OpCode op, RegIdx rs, RegIdx rt, Label *label);
  Instr(int line, OpCode op, Label *label);
  Instr(int line, OpCode op, RegIdx pc, RegIdx seg);

  void insert_noop(int count);
  int no_ops() { return no_ops_; }

  OpCode op() { return op_; }

  int line() const { return line_; }
  void set_line(int line) { line_ = line; }

  bool is_jump();

  std::optional<RegIdx> src_reg1();
  std::optional<RegIdx> src_reg2();
  std::optional<RegIdx> dest_reg();

  Instr *next() const { return next_; }
  void set_next(Instr *next) { next_ = next; }
  Instr *prev() const { return prev_; }
  void set_prev(Instr *prev) { prev_ = prev; }

  std::string to_hex_str(bool c_pref = false);
  std::string to_string();

private:
  int no_ops_ = 0;
  std::optional<RegIdx> r1_, r2_, r3_;
  std::optional<int> imd_;
  std::optional<Label *> label_;

  Instr *next_ = nullptr, *prev_ = nullptr;

  OpCode op_;
  InstrType type_;
  int line_;
};
