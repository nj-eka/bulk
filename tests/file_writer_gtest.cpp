#include <gtest/gtest.h>

#include "bulk/bulk_event.hpp"
#include "cmd/token_event.hpp"
#include "logs/file_writer.hpp"

TEST(file_writer, init) {
  const std::string local_name = "test";
  const std::string output_dir = ".";

  auto writer = logs::FileWriter::create(output_dir, local_name);
  ASSERT_FALSE(!writer);
  ASSERT_EQ(core::SubscriptionStatus::READY, writer->getSubscriptionStatus());
  ASSERT_EQ(writer->getSubscriberID(), std::format("{}.{}.{}", typeid(*(writer.get())).name(),
                                                   std::filesystem::canonical(output_dir).c_str(), local_name));
}

// TODO: add mocking tests for checking file creation, writing etc...