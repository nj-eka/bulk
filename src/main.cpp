#include "args/options.hpp"
#include "bulk/provider/bulk_translator.hpp"
#include "cmd/provider/console_parser.hpp"
#include "io/redirection.hpp"
#include "logs/file_writer.hpp"
#include "logs/ostream_writer.hpp"
#include "utils/defer.hpp"

namespace fs = std::filesystem;

/**
 * @brief course c++ developer: hw7 - bulk
 *
 * [task](task7_cmd.pdf)
 *
 * ![Class diagram](puml/class_diagram.svg)
 * ![Sequence diagram](puml/sequence_diagram.svg)
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char const *argv[]) {
  try {
    auto [desc, vm] = args::parse(argc, argv);
    if (args::info_requested(vm, desc)) {
      return EAGAIN;
    }
    const std::unique_ptr<io::istream2file_redirect> in_guard =
        vm.count("input_file") ? std::make_unique<io::istream2file_redirect>(vm["input_file"].as<std::string>())
                               : nullptr;
    fs::create_directories(vm.count("output_dir") ? vm["output_dir"].as<std::string>() : "./");

    auto &console_parser = cmd::provider::ConsoleParser::instance();
    utils::defer d0{[&] { console_parser.clear(); }};
    {  // inner scope is for just to show persistent type of subscription
      auto translator = bulk::provider::BulkTranslator::create(vm["number"].as<size_t>());
      auto console_writer = logs::OutputStreamWriter::create(std::cout);
      // console_parser.subscribe(console_writer, true); //!< optional, not required in hw
      console_parser.subscribe(translator, true);
      translator->subscribe(console_writer, true);
      translator->subscribe(logs::FileWriter::create("first", vm["output_dir"].as<std::string>()));
    }
    console_parser.run();
  } catch (const args::opt::error &e) {
    std::cerr << "Invalid command args / ERROR: " << e.what() << " (see --help)" << std::endl;
    return EINVAL;
  } catch (const std::system_error &e) {
    std::cerr << e.what() << " (" << e.code() << ")" << std::endl;
    return e.code().value();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return ENOEXEC;
  }
  return EXIT_SUCCESS;
}
