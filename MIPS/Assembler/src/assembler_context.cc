#include "assembler_context.h"

#include <cassert>
#include <fstream>

AssemblerContext::AssemblerContext(FILE *in) : in_file_(in) {}

void AssemblerContext::add_token(Token token) {
  current_line_.push_back(std::move(token));
}

void AssemblerContext::new_line(int line) {
  if (current_line_.empty()) {
    return;
  }
  auto op = current_line_[0];
  if (op.is_opcode()) {
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
      break;
      /* s type */
    case OpCode::SRL:
    case OpCode::SLL:
      assert(current_line_.size() == 4);
      assert(current_line_[1].is_reg());
      assert(current_line_[2].is_reg());
      assert(current_line_[3].is_imd());
      add_instr(Instr(line, op.opcode(), current_line_[1].reg(),
                      current_line_[2].reg(), current_line_[3].imd()));
      break;
      /* i type */
    case OpCode::ADDI:
    case OpCode::ANDI:
    case OpCode::SUBI:
    case OpCode::ORI:
      assert(current_line_.size() == 4);
      assert(current_line_[1].is_reg());
      assert(current_line_[2].is_reg());
      assert(current_line_[3].is_imd());
      add_instr(Instr(line, op.opcode(), current_line_[1].reg(),
                      current_line_[2].reg(), current_line_[3].imd()));
      break;
    case OpCode::LW:
    case OpCode::SW:
      assert(current_line_.size() == 4);
      assert(current_line_[1].is_reg());
      assert(current_line_[2].is_imd());
      assert(current_line_[3].is_reg());
      add_instr(Instr(line, op.opcode(), current_line_[1].reg(),
                      current_line_[3].reg(), current_line_[2].imd()));
      break;
    case OpCode::BNEQ:
    case OpCode::BEQ:
      assert(current_line_.size() == 4);
      assert(current_line_[1].is_reg());
      assert(current_line_[2].is_reg());
      assert(current_line_[3].is_label());
      add_instr(Instr(line, op.opcode(), current_line_[1].reg(),
                      current_line_[2].reg(), current_line_[3].label()));
      break;
    /* j type */
    case OpCode::J:
      assert(current_line_.size() == 2);
      assert(current_line_[1].is_label());
      add_instr(Instr(line, op.opcode(), current_line_[1].label()));
      break;
      /* jr type */
    case OpCode::JR:
      assert(current_line_.size() == 3);
      assert(current_line_[1].is_reg());
      assert(current_line_[2].is_reg());
      add_instr(Instr(line, op.opcode(), current_line_[1].reg(),
                      current_line_[2].reg()));
      break;
      /* no-op */
    case OpCode::NOP:
      assert(current_line_.size() == 0);
      add_instr(Instr(line));
      break;
    }
  } else {
    // surely is a label
    assert(current_line_[0].is_label());
    if (current_block_) {
      blocks_.push_back(std::move(current_block_));
    }
    current_block_ = std::make_unique<Block>(current_line_[0].label());
  }
  current_line_.clear();
}

Label *AssemblerContext::get_label(std::string_view label) {
  std::string key = std::string(label);
  if (!labels_.contains(key)) {
    labels_.emplace(key, std::make_unique<Label>(std::string(label)));
  }
  return labels_.at(key).get();
}

void AssemblerContext::add_instr(Instr instr) {
  if (!current_block_) {
    current_block_ = std::make_unique<Block>();
  }
  bool jump = instr.is_jump();
  current_block_->add_instr(std::move(instr));
  if (jump) {
    blocks_.push_back(std::move(current_block_));
    current_block_ = nullptr;
  }
}

void AssemblerContext::scan() {
  init_scanner();
  finish_scanner();

  if (current_block_) {
    blocks_.push_back(std::move(current_block_));
  }

  process();
}

void AssemblerContext::process() {
  int pc = 0;
  for (auto &block : blocks_) {
    if (block->label()) {
      block->label()->set_address(pc);
    }
    auto out = block->out();
    for (auto &inst : out) {
      out_lines_.push_back(std::move(inst));
      out_lines_.back().set_line(pc);
      pc++;
    }
  }
}

void AssemblerContext::gen_asm(const char *path) {
  std::ofstream out(path);
  for (auto &instr : out_lines_) {
    out << instr.to_string() << std::endl;
  }
}

void AssemblerContext::gen_hex(const char *path) {
  std::ofstream out(path);
  for (auto &instr : out_lines_) {
    out << instr.to_hex_str() << std::endl;
  }
}

void AssemblerContext::gen_c_array(const char *path) {
  std::ofstream out(path);
  out << "unsigned short asm_code[] = "
      << "[";
  for (auto &instr : out_lines_) {
    out << instr.to_hex_str(true);
  }
  out << "];";
}
