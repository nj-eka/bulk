#pragma once

#include <type_traits>
#include <variant>
#include <vector>

#include "../../cmd/token_event.hpp"
#include "../../core/publisher.hpp"
#include "../../core/subscriber.hpp"
#include "../../utils/overload.hpp"
#include "../bulk_event.hpp"

namespace bulk::provider {

class BulkTranslator : public core::SubscriberBase<cmd::TokenEvent>,
                       public core::PublisherMixin<BulkEvent>,
                       public std::enable_shared_from_this<BulkTranslator> {
 public:
  [[nodiscard]] static std::shared_ptr<BulkTranslator> create(size_t BULK_SIZE) {
    return std::shared_ptr<BulkTranslator>(new BulkTranslator(BULK_SIZE));
  }

  core::SubscriptionStatus getSubscriptionStatus() noexcept override {
    return (std::holds_alternative<State::Base>(_state) || std::holds_alternative<State::Block>(_state))
               ? core::SubscriptionStatus::READY
               : core::SubscriptionStatus::CANCEL;
  }
  // clang-format off
  core::SubscriptionStatus onEvent(cmd::TokenEvent const& event) override {
    _state = std::visit(
      utils::overload{
        // Base:
        [&](State::Base & state, cmd::TokenCommand const& tok) -> VState {
          if (_bulk.commands.size() == 0)
            _bulk.time_stamp = std::chrono::system_clock::now();
          _bulk.commands.emplace_back(tok.value);
          if (_bulk.commands.size() >= _BULK_SIZE){
            notify(_bulk);
            _bulk.commands.clear();
          }
          return state;
        },
        [&](State::Base &, cmd::TokenBlockBegin const&) -> VState {
          if (_bulk.commands.size()){
            notify(_bulk);
            _bulk.commands.clear();
          }
          return State::Block{1}; 
        },
        [&](State::Base, cmd::TokenBlockEnd const&) -> VState {
          _bulk.commands.clear();
          clear(); // subs
          return State::Failed{}; 
        },        
        [&](State::Base, cmd::TokenEnd const&) -> VState {
          if (_bulk.commands.size()){
            notify(_bulk);          
            _bulk.commands.clear();
          }         
          clear(); // subs
          return State::Finish{}; 
        },
        // Block:
        [&](State::Block & state, cmd::TokenCommand const& tok) -> VState {
          if (_bulk.commands.size() == 0)
            _bulk.time_stamp = std::chrono::system_clock::now();
          _bulk.commands.emplace_back(tok.value);
          return state;
        },
        [&](State::Block & state, cmd::TokenBlockBegin const&) -> VState {
          ++state.level;
          return state; 
        },
        [&](State::Block & state, cmd::TokenBlockEnd const&) -> VState {
          --state.level;
          if (state.level == 0){
            notify(_bulk);
            _bulk.commands.clear();
            return State::Base{}; 
          }
          return state; 
        },
        [&](State::Block, cmd::TokenEnd const&) -> VState {
          _bulk.commands.clear();
          clear(); // subs
          return State::Finish{}; 
        },
        // TODO: resolve case - call at failed or finished state... maybe throw an exception?
        [&](auto & state, auto const&) -> VState {
          _bulk.commands.clear();          
          clear(); // subs
          return state; 
        },
      }, _state, event.token);
      return getSubscriptionStatus();
  }
  // clang-format on

 private:
  const size_t _BULK_SIZE;

  BulkTranslator(size_t BULK_SIZE) : _BULK_SIZE{BULK_SIZE}, _state{State::Base{}} {
    _bulk.commands.reserve(_BULK_SIZE);
  }

  struct State {
    struct Base {};
    struct Block {
      size_t level = 0;
    };
    struct Failed {};
    struct Finish {};
  };

  using VState = std::variant<State::Base, State::Block, State::Failed, State::Finish>;

  VState _state;
  BulkEvent _bulk;
};

}  // namespace bulk::provider