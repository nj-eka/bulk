#pragma once

#include <format>
#include <typeinfo>

namespace core {

/**
 * @brief used to manage subscription state
 *
 */
enum class SubscriptionStatus : char {
  READY,    //!< accept event
  SUSPEND,  //!< do not receive event but keep subscription
  CANCEL    //!< unsubscribe (asap)
};

template <typename EventType>
struct SubscriberBase {
  virtual ~SubscriberBase() = default;
  /**
   * @brief Main Subscription Method
   *
   * if exception is thrown then publisher will unsubscribe this subscriber on its side imediatly
   * but to unsubscribe in normal way it's better just to return 'SubscriptionStatus::CANCEL' status
   *
   * @param event
   * @return SubscriptionStatus state after handling `onEvent` method
   */
  virtual SubscriptionStatus onEvent(EventType const& event) = 0;
  /**
   * @brief Get Subscriber current State object
   *
   * used by publisher before calling `onEvent` method
   * state management is done in derived classes as needed (see `file_writer.hpp`)
   *
   * @return SubscriptionStatus
   */
  virtual SubscriptionStatus getSubscriptionStatus() noexcept { return SubscriptionStatus::READY; }
  /**
   * @brief Get the Subscriber Id object (internal usage for subsciber identification)
   *
   * = {`typeid(*this).name()`}.{`this->getSubscriberLocalName()`}
   *
   * @return std::string
   */
  virtual std::string getSubscriberID() final {
    if (_id == "") _id = std::format("{}.{}", typeid(*this).name(), this->getSubscriberName());
    return _id;
  }
  // virtual bool operator<(SubscriberBase const& rhs) const final { return getSubscriberID() < rhs.getSubscriberID(); }

 protected:
  /**
   * @brief Get Subscriber Name (local within derived type)
   *
   * applied as a suffix to `typeid(*this)` to identify subscribers of the same type
   *
   * @return * std::string
   */
  virtual std::string getSubscriberName() const noexcept { return ""; }

 private:
  std::string _id = "";
};

}  // namespace core