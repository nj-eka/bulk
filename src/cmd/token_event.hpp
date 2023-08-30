#pragma once

#include "token.hpp"

namespace cmd {

struct TokenEvent {
  Token token;
  size_t index;
};

inline void print(TokenEvent const& event, std::ostream& out) {
  out << "#" << event.index << " > " << to_string(event.token) << std::endl;
}

}  // namespace cmd