#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

struct Component;

struct SubComponent {
  Component *comp;
  int count;
};

struct Component {
  std::string name;
  std::vector<SubComponent> subcomps;
};

struct Context {

  Component *get_component(std::string name) {
    if (table.contains(name)) {
      return &table.at(name);
    } else {
      auto [itr, inst] = table.emplace(name, Component{.name = name});
      assert(inst);
      return &(itr->second);
    }
  }

  void parse_line(std::string_view line) {
    auto start = line.find_first_not_of(" \t\n\r");
    if (start == line.npos) {
      return;
    }
    line.remove_prefix(start);

    auto name_end = line.find_first_of(" =:\t\n");
    if (name_end == line.npos) {
      return;
    }

    std::string_view name = line.substr(0, name_end);

    for (size_t i = 0; i < line.size(); i++) {
      char ch = line[i];
      if (ch == ':') {
        current_comp = get_component(std::string(name));
        return;
      } else if (ch == '=') {
        line.remove_prefix(i + 1);
        assert(current_comp);

        auto subcomp = get_component(std::string(name));
        int num = std::atoi(line.data());
        current_comp->subcomps.push_back(
            SubComponent{.comp = subcomp, .count = num});
        return;
      }
    }
  }

  void space(int n) {
    for (int i = 0; i < n; i++) {
      std::cout << "  ";
    }
  }

  std::unordered_map<Component *, int> count_ic(Component *comp, int level) {
    std::unordered_map<Component *, int> ret;

    if (comp->subcomps.empty()) {
      ret[comp] = 1;
    } else {
      for (auto &sc : comp->subcomps) {
        auto counts = count_ic(sc.comp, level + 1);
        for (auto &[scc, n] : counts) {
          ret[scc] += sc.count * n;
        }
      }
    }

    space(level);
    std::cout << "[" << comp->name << "]" << std::endl;
    for (auto &[c, n] : ret) {
      space(level);
      std::cout << c->name << ": " << n << std::endl;
    }

    return ret;
  }

  std::unordered_map<std::string, Component> table;
  Component *current_comp = nullptr;
};

int main() {
  std::ifstream in("ic_count.txt");
  std::string line;
  Context context;
  while (std::getline(in, line)) {
    context.parse_line(line);
  }

  auto counts = context.count_ic(context.get_component("FPA"), 0);
  // for (auto &[c, n] : counts) {
  //   std::cout << c->name << ": " << n << std::endl;
  // }
}
