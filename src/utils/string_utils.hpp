#pragma once

// #include <cctype>
// #include <ranges>
#include <algorithm>
#include <string>
#include <string_view>

namespace utils {

// std::string& trim2(std::string& str) {
//   auto view = str | std::views::drop_while(isspace) | std::views::reverse | std::views::drop_while(isspace) |
//                std::views::reverse;
//     auto [in, out] = std::ranges::copy(view, str.begin());
//     str.erase(out, str.end());
//     return str;
// }

/** @name with std::string */
///@{
inline std::string& ltrim(std::string& str, std::string const& whitespace = " \r\n\t\v\f") {
  str.erase(0, str.find_first_not_of(whitespace));
  return str;
}

inline std::string& rtrim(std::string& str, std::string const& whitespace = " \r\n\t\v\f") {
  str.erase(str.find_last_not_of(whitespace) + 1);
  return str;
}

inline std::string& trim(std::string& str, std::string const& whitespace = " \r\n\t\v\f") {
  return rtrim(ltrim(str, whitespace), whitespace);
}

inline std::string& to_lower(std::string& str) {
  std::transform(str.cbegin(), str.cend(), str.begin(), ::tolower);
  return str;
}

inline std::string& to_upper(std::string& str) {
  std::transform(str.cbegin(), str.cend(), str.begin(), ::toupper);
  return str;
}
///@}

/** @name with std::string_view */
///@{
inline std::string_view ltrim(std::string_view str, std::string_view whitespace = " \r\n\t\v\f") {
  auto const pos(str.find_first_not_of(whitespace));
  str.remove_prefix(std::min(pos, str.length()));
  return str;
}

inline std::string_view rtrim(std::string_view str, std::string_view whitespace = " \r\n\t\v\f") {
  auto const pos(str.find_last_not_of(whitespace));
  str.remove_suffix(std::min(str.length() - pos - 1, str.length()));
  return str;
}

inline std::string_view trim(std::string_view str, std::string_view whitespace = " \r\n\t\v\f") {
  return rtrim(ltrim(str, whitespace), whitespace);
}
///@}

}  // namespace utils