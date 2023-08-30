#include <gtest/gtest.h>

#include "bulk/provider/bulk_translator.hpp"
#include "cmd/token_event.hpp"
#include "logs/ostream_writer.hpp"

TEST(_bulk_translator, subs) {
  auto translator = bulk::provider::BulkTranslator::create(2);
  ASSERT_FALSE(!translator);
  ASSERT_EQ(core::SubscriptionStatus::READY, translator->getSubscriptionStatus());
  ASSERT_EQ(translator->getSubscriberID(), std::format("{}.{}", typeid(*(translator.get())).name(), ""));
  ASSERT_FALSE(translator->hasSubscription());

  translator->subscribe(logs::OutputStreamWriter::create(std::cout), true);
  ASSERT_TRUE(translator->hasSubscription());

  translator->clear();
  ASSERT_FALSE(translator->hasSubscription());
}

TEST(_bulk_translator, pub_base) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenCommand{"cmd1"}, 1});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenCommand{"cmd2"}, 2});
  bulk::print(bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}}}, eta);
  ASSERT_EQ(out.str(), eta.str());
  ASSERT_TRUE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::READY, translator->getSubscriptionStatus());
}

TEST(_bulk_translator, pub_base_eof) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenCommand{"cmd1"}, 1});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenEnd{}, 2});
  bulk::print(bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}}}, eta);
  ASSERT_EQ(out.str(), eta.str());
  ASSERT_FALSE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::CANCEL, translator->getSubscriptionStatus());
}

TEST(_bulk_translator, pub_base_block_begin) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenCommand{"cmd1"}, 1});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenBlockBegin{}, 2});
  bulk::print(bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}}}, eta);
  ASSERT_EQ(out.str(), eta.str());
  ASSERT_TRUE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::READY, translator->getSubscriptionStatus());
}

TEST(_bulk_translator, pub_base_block_end) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenCommand{"cmd1"}, 1});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenBlockEnd{}, 2});
  ASSERT_EQ(out.str(), "");
  ASSERT_FALSE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::CANCEL, translator->getSubscriptionStatus());
}

TEST(_bulk_translator, pub_block_base) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenBlockBegin{}, 1});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenCommand{"cmd1"}, 2});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenCommand{"cmd2"}, 3});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenCommand{"cmd3"}, 4});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenBlockEnd{}, 5});
  bulk::print(bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}, {"cmd3"}}}, eta);
  ASSERT_EQ(out.str(), eta.str());
  ASSERT_TRUE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::READY, translator->getSubscriptionStatus());
}

TEST(_bulk_translator, pub_block_eof) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenBlockBegin{}, 1});
  translator->onEvent({cmd::TokenCommand{"cmd1"}, 2});
  translator->onEvent({cmd::TokenCommand{"cmd2"}, 3});
  translator->onEvent({cmd::TokenCommand{"cmd3"}, 4});
  translator->onEvent({cmd::TokenEnd{}, 5});
  ASSERT_EQ(out.str(), "");
  ASSERT_FALSE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::CANCEL, translator->getSubscriptionStatus());
}

TEST(_bulk_translator, pub_block_nested) {
  auto translator = bulk::provider::BulkTranslator::create(2);

  std::stringstream out, eta;
  translator->subscribe(logs::OutputStreamWriter::create(out), true);

  translator->onEvent({cmd::TokenBlockBegin{}, 1});
  translator->onEvent({cmd::TokenCommand{"cmd1"}, 2});
  translator->onEvent({cmd::TokenCommand{"cmd2"}, 3});
  translator->onEvent({cmd::TokenBlockBegin{}, 4});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenCommand{"cmd3"}, 5});
  translator->onEvent({cmd::TokenCommand{"cmd4"}, 6});
  translator->onEvent({cmd::TokenBlockEnd{}, 7});
  ASSERT_EQ(out.str(), "");
  translator->onEvent({cmd::TokenCommand{"cmd5"}, 8});
  translator->onEvent({cmd::TokenBlockEnd{}, 9});
  bulk::print(
      bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}, {"cmd3"}, {"cmd4"}, {"cmd5"}}},
      eta);
  ASSERT_EQ(out.str(), eta.str());
  ASSERT_TRUE(translator->hasSubscription());
  ASSERT_EQ(core::SubscriptionStatus::READY, translator->getSubscriptionStatus());
}