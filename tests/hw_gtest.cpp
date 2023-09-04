#include <gtest/gtest.h>

#include <sstream>

#include "bulk/provider/bulk_translator.hpp"
#include "cmd/provider/console_parser.hpp"
#include "io/redirection.hpp"
#include "logs/ostream_writer.hpp"
#include "utils/defer.hpp"

TEST(hw, use_case_1) {
  // auto const ifguard = std::make_unique<io::istream2file_redirect>(std::getenv("EXAMPLE1"));
  std::stringstream in, out, eta;
  const std::unique_ptr<io::istream2istream_redirect> in_guard = std::make_unique<io::istream2istream_redirect>(in);

  auto &parser = cmd::provider::ConsoleParser::instance();
  utils::defer d0{[&] { parser.clear(); }};
  {
    auto translator = bulk::provider::BulkTranslator::create(3);
    auto writer = logs::OutputStreamWriter::create(out);
    parser.subscribe(translator, true);
    translator->subscribe(writer, true);
  }
  in << "cmd1\ncmd2\ncmd3\ncmd4\ncmd5";
  parser.run();

  eta << bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}, {"cmd3"}}} << std::endl;
  eta << bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd4"}, {"cmd5"}}} << std::endl;

  ASSERT_EQ(out.str(), eta.str());
}

TEST(hw, use_case_2) {
  // auto const ifguard = std::make_unique<io::istream2file_redirect>(std::getenv("EXAMPLE2"));
  std::stringstream in, out, eta;
  const std::unique_ptr<io::istream2istream_redirect> in_guard = std::make_unique<io::istream2istream_redirect>(in);

  auto &parser = cmd::provider::ConsoleParser::instance();
  utils::defer d0{[&] { parser.clear(); }};
  {
    auto translator = bulk::provider::BulkTranslator::create(3);
    auto writer = logs::OutputStreamWriter::create(out);
    parser.subscribe(translator, true);
    translator->subscribe(writer, true);
  }
  in << "cmd1\ncmd2\n{\ncmd3\ncmd4\n}\n{\ncmd5\ncmd6\n{\ncmd7\ncmd8\n}\ncmd9\n}\n{\ncmd10\ncmd11";
  parser.run();

  eta << bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}}} << std::endl;
  eta << bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd3"}, {"cmd4"}}} << std::endl;
  eta << bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd5"}, {"cmd6"}, {"cmd7"}, {"cmd8"}, {"cmd9"}}} << std::endl;

  ASSERT_EQ(out.str(), eta.str());
}

// int main(int argc, char const *argv[]){
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }