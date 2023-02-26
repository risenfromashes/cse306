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
        instr.set_prev(l[src]);
        instr.insert_noop(2);
      }
      r[src] = &instr;
    }

    if (instr.src_reg2()) {
      int src = (int)*instr.src_reg2();
      std::cerr << "src2: " << to_string(*instr.src_reg2()) << std::endl;
      if (l[src]) {
        l[src]->set_next(&instr);
        instr.set_prev(l[src]);
        instr.insert_noop(2);
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
  int pc = 0;
  out.push_back(Instr::noop());
  for (auto &instr : instrs_) {
    int cur = pc;
    /* pop unexec instructions with expired next lines */
    while (!unexec.empty() && unexec.top()->next() &&
           unexec.top()->next()->line() <=
               (unexec.top()->line() + instr.no_ops())) {
      auto top = unexec.top();
      unexec.pop();
      out.push_back(*top);
      out.back().set_line(pc);
      pc++;
    }
    /* print necessary noops first */
    if (instr.prev()) {
      int no_ops =
          std::max(std::min(instr.no_ops(),
                            instr.no_ops() - cur + instr.prev()->line() + 1),
                   0);
      std::cout << "nops: " << no_ops << std::endl;
      while (no_ops && !unexec.empty()) {
        auto top = unexec.top();
        unexec.pop();
        out.push_back(*top);
        out.back().set_line(pc);
        pc++;
        no_ops--;
      }
      for (int i = 0; i < no_ops; i++) {
        out.push_back(Instr::noop());
        out.back().set_line(pc);
        pc++;
      }
    }
    /* now handle current instruction */
    if (instr.is_jump()) {

      /* can't reorder jump instructions */
      while (!unexec.empty()) {
        auto top = unexec.top();
        unexec.pop();
        out.push_back(*top);
        out.back().set_line(pc);
        pc++;
      }
      out.push_back(instr);
      out.back().set_line(pc);
      pc++;

    } else if (!instr.next()) {
      /* no dependent instruction, can put anywhere */
      unexec.push(&instr);
    } else {
      /* no room for reordering */
      if ((instr.next()->line() - instr.line()) <= (instr.no_ops() + 1)) {
        /* so output immediately */
        out.push_back(instr);
        out.back().set_line(pc);
        pc++;
      } else {
        unexec.push(&instr);
      }
    }
  }
  /* print remaining unexecuted instructions */
  while (!unexec.empty()) {
    auto top = unexec.top();
    unexec.pop();
    out.push_back(*top);
    out.back().set_line(pc);
    pc++;
  }
  return out;
}
