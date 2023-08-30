#include <gtest/gtest.h>

#include "bulk/bulk_event.hpp"
#include "cmd/token_event.hpp"
#include "logs/ostream_writer.hpp"

TEST(ostream_writer, init) {
  const std::string local_name = "test";
  std::ostream& out = std::cout;
  auto writer = logs::OutputStreamWriter::create(std::cout, local_name);
  ASSERT_FALSE(!writer);
  ASSERT_EQ(core::SubscriptionStatus::READY,
            static_cast<core::SubscriberBase<cmd::TokenEvent>*>(writer.get())->getSubscriptionStatus());
  ASSERT_EQ(core::SubscriptionStatus::READY,
            static_cast<core::SubscriberBase<bulk::BulkEvent>*>(writer.get())->getSubscriptionStatus());
  ASSERT_EQ(static_cast<core::SubscriberBase<cmd::TokenEvent>*>(writer.get())->getSubscriberID(),
            static_cast<core::SubscriberBase<bulk::BulkEvent>*>(writer.get())->getSubscriberID());
  ASSERT_EQ(static_cast<core::SubscriberBase<bulk::BulkEvent>*>(writer.get())->getSubscriberID(),
            std::format("{}.{}.{}", typeid(*(writer.get())).name(), typeid(out).name(), local_name));
}