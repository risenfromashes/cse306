#pragma once

#include <cstdio>
#include <vector>

#include "token.h"

class AssemblerContext {
public:
  void init_scanner();
  void finish_scanner();

  void add_token(Token token);

  void new_line(int line);

private:
  void *scanner_;
  FILE *in_file_;
  std::vector<Token> current_line_;
};
