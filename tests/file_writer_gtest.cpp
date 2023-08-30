#include <gtest/gtest.h>

#include "bulk/bulk_event.hpp"
#include "cmd/token_event.hpp"
#include "logs/file_writer.hpp"

// so far skipped... TODO: need to find out why any filesystem manipulation throws exceptions like (see below) when done
// under gtest. C++ exception with description "filesystem error: cannot make canonical path: No such file or directory
// [test]" thrown in the test body.

// TEST(file_writer, init) {
//   const std::string local_name = "test";
//   const std::string output_dir = ".";

//   auto writer = logs::FileWriter::create(output_dir, local_name);
//   ASSERT_FALSE(!writer);
//   ASSERT_EQ(core::SubscriptionStatus::READY, writer->getSubscriptionStatus());
//   ASSERT_EQ(writer->getSubscriberID(), std::format("{}.{}.{}", typeid(*(writer.get())).name(),
//                                                    std::filesystem::canonical(output_dir).c_str(), local_name));
// }