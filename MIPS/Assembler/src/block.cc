#include "block.h"

#include <array>
#include <queue>

Block::Block() : label_(nullptr) {}
Block::Block(Label *label) : label_(label) {}

void Block::add_instr(Instr instr) { instrs_.push_back(std::move(instr)); }

void Block::find_nexts() {
  std::array<Instr *, N_REGISTER> l, r;
  for (int i = 0; i < N_REGISTER; i++) {
    l[i] = r[i] = nullptr;
  }

  for (auto &instr : instrs_) {
    std::cerr << "[" << instr.to_string() << "]" << std::endl;
    if (instr.src_reg1()) {
      int src = (int)*instr.src_reg1();
      std::cerr << "src1: " << to_string(*instr.src_reg1()) << std::endl;
      if (l[src]) {
        l[src]->set_next(&instr);
        if (instr.line() - l[src]->line() <= 2) {
          instr.insert_noop(3 - instr.line() + l[src]->line());
        }
      }
      r[src] = &instr;
    }

    if (instr.src_reg2()) {
      int src = (int)*instr.src_reg2();
      std::cerr << "src2: " << to_string(*instr.src_reg2()) << std::endl;
      if (l[src]) {
        l[src]->set_next(&instr);
        if (instr.line() - l[src]->line() <= 2) {
          instr.insert_noop(3 - instr.line() + l[src]->line());
        }
      }
      r[src] = &instr;
    }

    if (instr.dest_reg()) {
      int dest = (int)*instr.dest_reg();
      std::cerr << "dest: " << to_string(*instr.dest_reg()) << std::endl;
      if (r[dest] && r[dest] != &instr) {
        r[dest]->set_next(&instr);
      }
      l[dest] = &instr;
    }

    for (int i = 0; i < N_REGISTER; i++) {
      if (l[i] || r[i]) {
        auto reg = (RegIdx)i;
        std::cerr << to_string(reg) << ": " << std::endl;
        if (l[i]) {
          std::cerr << "L: " << l[i]->to_string() << std::endl;
        }
        if (r[i]) {
          std::cerr << "R: " << r[i]->to_string() << std::endl;
        }
        std::cerr << std::endl;
      }
    }
  }

  for (auto &instr : instrs_) {
    std::cerr << instr.to_string() << " -> ";
    if (instr.next()) {
      std::cerr << instr.next()->to_string();
    }
    std::cerr << std::endl;
  }
}

std::vector<Instr> Block::out() {
  find_nexts();
  /* sort according to earliest next line */
  std::priority_queue<Instr *, std::vector<Instr *>,
                      decltype([](const Instr *a, const Instr *b) {
                        if (!a->next()) {
                          return true;
                        }
                        if (!b->next()) {
                          return false;
                        }
                        return a->next()->line() > b->next()->line();
                      })>
      unexec;

  std::vector<Instr> out;
  for (auto &instr : instrs_) {
    int cur = instr.line();
    /* print necessary noops first */
    int no_ops = instr.no_ops();
    while (no_ops && !unexec.empty()) {
      auto top = unexec.top();
      unexec.pop();
      std::cerr << "1. Popped: " << top->to_string() << " " << top << std::endl;
      out.push_back(*top);
      no_ops--;
    }
    for (int i = 0; i < no_ops; i++) {
      out.push_back(Instr::noop());
    }
    /* pop unexec instructions with expired next lines */
    while (!unexec.empty() && unexec.top()->next() &&
           unexec.top()->next()->line() <= cur) {
      auto top = unexec.top();
      unexec.pop();
      std::cerr << "2. Popped: " << top->to_string() << " " << top << std::endl;
      out.push_back(*top);
    }
    /* now handle current instruction */
    if (!instr.next()) {
      /* no dependent instruction, can put anywhere */
      std::cerr << "1. Pushing: " << instr.to_string() << " " << &instr
                << std::endl;
      unexec.push(&instr);
    } else {
      /* no room for reordering */
      if ((instr.next()->line() - instr.line()) <= (instr.no_ops() + 1)) {
        /* so output immediately */
        out.push_back(instr);
      } else {
        /* this can be put instead of a later no-op */
        std::cerr << "2. Pushing: " << instr.to_string() << " " << &instr
                  << std::endl;
        unexec.push(&instr);
      }
    }
  }
  /* print remaining unexecuted instructions */
  while (!unexec.empty()) {
    auto &top = unexec.top();
    unexec.pop();
    out.push_back(*top);
  }
  return out;
}
