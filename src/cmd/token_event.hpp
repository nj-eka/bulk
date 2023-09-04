#pragma once

#include "token.hpp"

namespace cmd {

struct TokenEvent {
  Token token;
  size_t index;
};

inline std::ostream& operator<<(std::ostream& out, TokenEvent const& event) {
  out << "#" << event.index << " > " << to_string(event.token);
  return out;
}

}  // namespace cmd