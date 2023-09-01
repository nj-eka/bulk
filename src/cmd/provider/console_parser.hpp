#pragma once

#include <iostream>

#include "core/publisher.hpp"
#include "utils/string_utils.hpp"
#include "cmd/token_event.hpp"

namespace cmd::provider {

/**
 * @brief Command parser from input stream
 *
 * to keep command stream consistency, parser must have exclusive access to stream resource
 *
 */
class ConsoleParser final : public core::PublisherMixin<TokenEvent> {
 public:
  static ConsoleParser &instance() {
    static ConsoleParser inst{};
    return inst;
  }

  ConsoleParser(ConsoleParser const &) = delete;
  ConsoleParser &operator=(ConsoleParser const &) = delete;
  ~ConsoleParser() {}

  /**
   * @brief parse all input stream passed to ctor while at least one subscriber is alive
   *
   */
  void run() {
    size_t index = 0;
    Token tok = TokenUnknown{};
    while (hasSubscription() && !std::holds_alternative<TokenEnd>(tok)) {
      tok = advance();
      if (!std::holds_alternative<TokenUnknown>(tok)) {
        notify(TokenEvent{tok, ++index});
      }
    }
  }

 private:
  ConsoleParser() noexcept {}

  Token advance() {
    if (std::string line; std::getline(std::cin, line)) {
      // utils::to_lower(utils::trim(line));
      utils::trim(line);
      if (line.size() > 0) {
        if (line.size() == 1) {
          switch (line[0]) {
            case '{':
              return TokenBlockBegin{};
            case '}':
              return TokenBlockEnd{};
            default:
              return TokenCommand{line};
          }
        } else
          return TokenCommand{line};
      } else
        return TokenUnknown{};
    } else
      return TokenEnd{};
  }
};

}  // namespace cmd::provider