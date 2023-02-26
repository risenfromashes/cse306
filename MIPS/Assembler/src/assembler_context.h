#pragma once

#include <cstdio>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "block.h"
#include "mips_instr.h"
#include "token.h"

class AssemblerContext {
public:
  AssemblerContext(FILE *in);
  void init_scanner();
  void finish_scanner();

  void scan();

  void gen_asm(const char *path);
  void gen_hex(const char *path);
  void gen_c_array(const char *path);

  void add_token(Token token);
  void add_instr(Instr instr);
  void add_label(std::string_view label);

  void new_line(int line);

  Label *get_label(std::string_view label);

private:
  void process();

  void *scanner_;
  FILE *in_file_;
  std::vector<Token> current_line_;
  std::unique_ptr<Block> current_block_;
  std::vector<std::unique_ptr<Block>> blocks_;
  std::vector<Instr> out_lines_;

  std::unordered_map<std::string, std::unique_ptr<Label>> labels_;
};
