#pragma once

#include <chrono>
#include <ostream>
#include <string_view>
#include <vector>

namespace bulk {

struct BulkEvent {
  std::chrono::system_clock::time_point time_stamp;
  std::vector<std::string> commands;
};

inline void print(BulkEvent const& event, std::ostream& out, std::string_view separator = ", ") {
  out << "bulk : ";
  auto from = event.commands.begin();
  auto to = event.commands.end();
  if (from != to) {
    out << *from;
    for (auto&& it = ++from; it != to; ++it) {
      out << separator << *it;
    }
  }
  out << '\n';
}

}  // namespace bulk