#include <gtest/gtest.h>

#include <sstream>

#include "bulk/provider/bulk_translator.hpp"
#include "cmd/provider/console_parser.hpp"
#include "io/redirection.hpp"
#include "logs/ostream_writer.hpp"
#include "utils/defer.hpp"
#include "utils/string_utils.hpp"

TEST(parser, singleton) {
  {
    auto &parser = cmd::provider::ConsoleParser::instance();
    ASSERT_FALSE(parser.hasSubscription());
    parser.subscribe(logs::OutputStreamWriter::create(std::cout));
    ASSERT_TRUE(parser.hasSubscription());
  }
  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_TRUE(parser.hasSubscription());

  parser.clear();
  ASSERT_FALSE(parser.hasSubscription());
}

TEST(parser, defer) {
  {
    auto &parser = cmd::provider::ConsoleParser::instance();
    ASSERT_FALSE(parser.hasSubscription());
    utils::defer d0{[&] { parser.clear(); }};
    ASSERT_FALSE(parser.hasSubscription());
    parser.subscribe(logs::OutputStreamWriter::create(std::cout));
    ASSERT_TRUE(parser.hasSubscription());
  }
  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_FALSE(parser.hasSubscription());
}

TEST(parser, subscribe_weak_with_own_false) {
  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_FALSE(parser.hasSubscription());
  utils::defer d0{[&] { parser.clear(); }};
  {
    auto writer = logs::OutputStreamWriter::create(std::cout);
    parser.subscribe(writer, false);
    ASSERT_TRUE(parser.hasSubscription());
  }
  ASSERT_FALSE(parser.hasSubscription());
}

TEST(parser, subscribe_shared_with_own_true) {
  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_FALSE(parser.hasSubscription());
  utils::defer d0{[&] { parser.clear(); }};
  {
    auto writer = logs::OutputStreamWriter::create(std::cout);
    parser.subscribe(writer, true);
    ASSERT_TRUE(parser.hasSubscription());
  }
  ASSERT_TRUE(parser.hasSubscription());
}

TEST(parser, subscribe_shared_with_move_semantic) {
  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_FALSE(parser.hasSubscription());
  utils::defer d0{[&] { parser.clear(); }};
  {
    parser.subscribe(logs::OutputStreamWriter::create(std::cout));
    ASSERT_TRUE(parser.hasSubscription());
  }
  ASSERT_TRUE(parser.hasSubscription());
}

TEST(parser, tokens) {
  std::stringstream in, out, eta;
  const std::unique_ptr<io::istream2istream_redirect> in_guard = std::make_unique<io::istream2istream_redirect>(in);

  auto &parser = cmd::provider::ConsoleParser::instance();
  utils::defer d0{[&] { parser.clear(); }};

  parser.subscribe(logs::OutputStreamWriter::create(out));
  ASSERT_TRUE(parser.hasSubscription()) << "failed subscription";

  in << "cmd0" << std::endl;
  in << "cmd1    " << std::endl;
  in << "    cmd2" << std::endl;
  in << "  cmd3  " << std::endl;
  in << "{  " << std::endl;
  in << " }" << std::endl;

  size_t index = 0;
  cmd::print(cmd::TokenEvent{cmd::TokenCommand{"cmd0"}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenCommand{"cmd1"}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenCommand{"cmd2"}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenCommand{"cmd3"}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenBlockBegin{}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenBlockEnd{}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenEnd{}, ++index}, eta);

  parser.run();
  ASSERT_TRUE(parser.hasSubscription()) << "failed subscription";
  ASSERT_EQ(out.str(), eta.str());

  parser.clear();
}

TEST(parser, bulk_translator) {
  std::stringstream in, out, eta;
  const std::unique_ptr<io::istream2istream_redirect> in_guard = std::make_unique<io::istream2istream_redirect>(in);

  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_FALSE(parser.hasSubscription());
  utils::defer d0{[&] { parser.clear(); }};

  auto translator = bulk::provider::BulkTranslator::create(2);
  ASSERT_FALSE(translator->hasSubscription());

  auto writer = logs::OutputStreamWriter::create(out);
  translator->subscribe(writer);
  ASSERT_TRUE(translator->hasSubscription());

  parser.subscribe(translator, false);
  ASSERT_TRUE(parser.hasSubscription());

  in << "cmd1" << std::endl;
  in << "cmd2" << std::endl;

  bulk::print(bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}}}, eta);
  parser.run();
  ASSERT_FALSE(parser.hasSubscription());

  ASSERT_EQ(out.str(), eta.str());
}

TEST(parser, writer_bulk_translator) {
  std::stringstream in, out, eta;
  const std::unique_ptr<io::istream2istream_redirect> in_guard = std::make_unique<io::istream2istream_redirect>(in);

  auto &parser = cmd::provider::ConsoleParser::instance();
  ASSERT_FALSE(parser.hasSubscription());
  utils::defer d0{[&] { parser.clear(); }};

  auto translator = bulk::provider::BulkTranslator::create(2);
  ASSERT_FALSE(translator->hasSubscription());

  auto writer = logs::OutputStreamWriter::create(out);
  parser.subscribe(writer, false);
  ASSERT_TRUE(parser.hasSubscription());

  translator->subscribe(writer);
  ASSERT_TRUE(translator->hasSubscription());

  parser.subscribe(translator, false);

  in << "cmd1" << std::endl;
  in << "cmd2" << std::endl;

  size_t index = 0;
  cmd::print(cmd::TokenEvent{cmd::TokenCommand{"cmd1"}, ++index}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenCommand{"cmd2"}, ++index}, eta);
  bulk::print(bulk::BulkEvent{std::chrono::system_clock::time_point(), {{"cmd1"}, {"cmd2"}}}, eta);
  cmd::print(cmd::TokenEvent{cmd::TokenEnd{}, ++index}, eta);

  parser.run();
  ASSERT_TRUE(parser.hasSubscription());
  ASSERT_EQ(out.str(), eta.str());
}