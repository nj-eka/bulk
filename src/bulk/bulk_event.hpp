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

inline std::ostream& operator<<(std::ostream& out, BulkEvent const& event) {
  out << "bulk : ";
  auto from = event.commands.begin();
  auto to = event.commands.end();
  if (from != to) {
    out << *from;
    for (auto&& it = ++from; it != to; ++it) {
      out << ", " << *it;
    }
  }
  return out;
}

}  // namespace bulk