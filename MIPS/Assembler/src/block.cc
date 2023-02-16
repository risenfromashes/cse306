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
    if (instr.src_reg1()) {
      int src = (int)*instr.src_reg1();
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
      if (r[dest]) {
        r[dest]->set_next(&instr);
      }
      l[dest] = &instr;
    }
  }
}

std::vector<Instr> Block::out() {
  find_nexts();
  /* sort according to earliest next line */
  std::priority_queue<Instr *, std::vector<Instr *>,
                      decltype([](const Instr *&a, const Instr *&b) {
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
    if (instr.no_ops() > 0) {
      int no_ops = instr.no_ops();
      while (!unexec.empty()) {
        auto &top = unexec.top();
        unexec.pop();
        out.push_back(*top);
        no_ops--;
      }
      for (int i = 0; i < no_ops; i++) {
        out.push_back(Instr::noop());
      }
    }
    /* pop unexec instructions with expired next lines */
    while (!unexec.empty() && unexec.top()->next()->line() <= cur) {
      auto &top = unexec.top();
      unexec.pop();
      out.push_back(*top);
    }
    /* now handle current instruction */
    if (!instr.next()) {
      /* no dependent instruction, can put anywhere */
      unexec.push(&instr);
    } else {
      /* no room for reordering */
      if (instr.next()->line() <= (instr.line() + instr.next()->no_ops())) {
        /* so output immediately */
        out.push_back(instr);
      } else {
        /* this can be put instead of a later no-op */
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
