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

constexpr int N_REGISTER = 16;
enum class RegIdx : int {
  ZERO,
  TO,
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

enum class InstrType { R, S, I, J, JR, NOP };

class Label {
public:
  Label() : id_(last_id_++) {}
  int id() { return id_; }

private:
  int id_;
  static inline int last_id_ = 0;
};

class Instr {
public:
  static Instr noop() { return Instr(0); }

  Instr(int line);
  Instr(int line, OpCode op, RegIdx rs, RegIdx rt, RegIdx rd);
  Instr(int line, OpCode op, RegIdx rs, RegIdx rt, int imd);
  Instr(int line, OpCode op, Label *label);
  Instr(int line, OpCode op, RegIdx pc, RegIdx seg);

  void insert_noop(int count);
  int no_ops() { return no_ops_; }

  int line() const { return line_; }
  void set_line(int line) { line_ = line; }

  bool is_jump();

  std::optional<RegIdx> src_reg1();
  std::optional<RegIdx> src_reg2();
  std::optional<RegIdx> dest_reg();

  Instr *next() const { return next_; }
  void set_next(Instr *next) { next_ = next; }

private:
  int no_ops_ = 0;
  std::optional<RegIdx> r1_, r2_, r3_;
  std::optional<int> imd_;
  std::optional<Label *> label_;

  Instr *next_ = nullptr;

  OpCode op_;
  InstrType type_;
  int line_;
};
