#pragma once

#include <iostream>
#include <memory>

#include "bulk/bulk_event.hpp"
#include "cmd/token_event.hpp"
#include "core/subscriber.hpp"

namespace logs {

class OutputStreamWriter final : public core::SubscriberBase<cmd::TokenEvent>,
                                 public core::SubscriberBase<bulk::BulkEvent>,
                                 public std::enable_shared_from_this<OutputStreamWriter> {
 public:
  [[nodiscard]] static std::shared_ptr<OutputStreamWriter> create(std::ostream& out, std::string_view local_name = "") {
    return std::shared_ptr<OutputStreamWriter>(new OutputStreamWriter(out, local_name));
  }

  core::SubscriptionStatus onEvent(bulk::BulkEvent const& event) override {
    bulk::print(event, _out);
    return static_cast<core::SubscriberBase<bulk::BulkEvent>*>(this)
        ->getSubscriptionStatus();  // == core::SubscriptionStatus::READY;
  }
  core::SubscriptionStatus onEvent(cmd::TokenEvent const& event) override {
    cmd::print(event, _out);
    return static_cast<core::SubscriberBase<cmd::TokenEvent>*>(this)
        ->getSubscriptionStatus();  // == core::SubscriptionStatus::READY;
  }

 private:
  std::ostream& _out;
  std::string const _name;

  OutputStreamWriter(std::ostream& out, std::string_view local_name)
      : _out{out}, _name{std::format("{}.{}", typeid(out).name(), local_name)} {}

  std::string getSubscriberName() const noexcept override { return _name; }
};

}  // namespace logs