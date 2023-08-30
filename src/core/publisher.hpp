#pragma once

#include <list>

#include "subscriber.hpp"

namespace core {

template <typename EventType>
class PublisherMixin {
 public:
  using Subscriber = SubscriberBase<EventType>;
  using WPtrSubscriber = std::weak_ptr<Subscriber>;
  using SPtrSubscriber = std::shared_ptr<Subscriber>;
  using VPtrSubscriber = std::variant<WPtrSubscriber, SPtrSubscriber>;

  virtual ~PublisherMixin() = default;

  void subscribe(SPtrSubscriber const& subscriber, bool own) {
    if (_subs.end() == _findSteady(subscriber->getSubscriberID())) {
      // _subs.emplace_back((own) ? subscriber : WPtrSubscriber(subscriber));
      if (own)
        _subs.emplace_back(subscriber);
      else
        _subs.emplace_back(WPtrSubscriber(subscriber));
    }
  }

  void subscribe(SPtrSubscriber&& subscriber) {
    if (_subs.end() == _findSteady(subscriber->getSubscriberID())) {
      _subs.emplace_back(std::forward<SPtrSubscriber>(subscriber));
    }
  }

  bool hasSubscription() {
    _refresh();
    return (_subs.size() > 0);
  }

  void unsubscribe(SPtrSubscriber const& subscriber) { _subs.erase(_findSteady(subscriber->getSubscriberID())); }

  void clear() { _subs.clear(); }

 protected:
  PublisherMixin() {}

  void notify(EventType const& event) {
    for (auto it = _subs.begin(); it != _subs.end();) {
      if (auto spo = _getShared(*it)) {
        auto& sp = spo.value();
        switch (sp->getSubscriptionStatus()) {
          case SubscriptionStatus::READY:
            try {
              if (SubscriptionStatus::CANCEL == sp->onEvent(event))
                it = _subs.erase(it);
              else
                ++it;
            } catch (...) {
              it = _subs.erase(it);
            }
            break;
          case SubscriptionStatus::SUSPEND:
            ++it;
            break;
          case SubscriptionStatus::CANCEL:
            it = _subs.erase(it);
            break;
        }
      } else {
        it = _subs.erase(it);
      }
    }
  }

 private:
  std::optional<SPtrSubscriber> _getShared(VPtrSubscriber const& vptr) {
    if (std::holds_alternative<WPtrSubscriber>(vptr)) {
      if (auto sp = std::get<WPtrSubscriber>(vptr).lock())
        return sp;
      else
        return std::nullopt;
    }
    return std::get<SPtrSubscriber>(vptr);
  }

  auto _findSteady(std::string_view id) {
    for (auto it = _subs.begin(); it != _subs.end();) {
      if (auto spo = _getShared(*it)) {
        if (spo.value()->getSubscriberID() == id) return it;
        ++it;
      } else {
        it = _subs.erase(it);
      }
    }
    return _subs.end();
  }

  void _refresh() {
    for (auto it = _subs.begin(); it != _subs.end();) {
      if (auto spo = _getShared(*it)) {
        if (SubscriptionStatus::CANCEL == spo.value()->getSubscriptionStatus())
          it = _subs.erase(it);
        else
          ++it;
      } else {
        it = _subs.erase(it);
      }
    }
  }

  std::list<VPtrSubscriber> _subs;  // TODO: move to std::forward_list if possible
};

}  // namespace core