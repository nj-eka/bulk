#pragma once

#include <format>
#include <ostream>
#include <string>
#include <variant>

#include "../utils/overload.hpp"

namespace cmd {

struct TokenBlockBegin {};  //!< {
struct TokenBlockEnd {};    //!< }
struct TokenCommand {
  std::string value;
};                       //!< any string (f.e. cmdN ...)
struct TokenUnknown {};  //!< in this task it's equal to empty (or with witespaces) or none string
struct TokenEnd {};      //!< EOF

using Token = std::variant<TokenBlockBegin, TokenBlockEnd, TokenCommand, TokenEnd, TokenUnknown>;

// clang-format off
inline std::string to_string(Token const& tok) {
  return std::visit(utils::overload{
      [](TokenBlockBegin const&) -> std::string { return "BLOCK_BEGIN ( { )"; },
      [](TokenBlockEnd const&) -> std::string { return "BLOCK_END ( } )"; },
      [](TokenCommand const& tok) -> std::string { return std::format("COMMAND ( {} )", tok.value); },
      [](TokenEnd const&) -> std::string { return "END ( . )"; },
      [](TokenUnknown const&) -> std::string { return "UNKNOWN ( ? )"; }
    }, tok);
}
// clang-format on

}  // namespace cmd