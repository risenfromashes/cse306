#include "assembler_context.h"
#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {

  const char *in = "in.asm";
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "-i") == 0) {
      if (i + 1 < argc) {
        in = argv[i + 1];
      }
    }
  }

  FILE *in_file = std::fopen(in, "r");
  if (!in_file) {
    std::cerr << "File " << in << " not found" << std::endl;
    return 1;
  }

  AssemblerContext assembler(in_file);
  assembler.scan();
  assembler.gen_asm("out.asm");
  assembler.gen_hex("out.hex");
}
