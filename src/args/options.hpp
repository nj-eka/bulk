#include <info/version.h>

#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
namespace args {

namespace opt = boost::program_options;
namespace fs = std::filesystem;

constexpr const size_t NUM = 3;
constexpr const unsigned DEFAULT_LINE_LENGTH = 120;

inline auto parse(int argc, char const *argv[]) {
  opt::options_description desc("Usage", DEFAULT_LINE_LENGTH);
  // clang-format off
  desc.add_options()
    ("help,h",      "Print this help message")
    ("version,v",   "Print version")
    ("info",        "Print project info")
    ("input_file,f",opt::value<std::string>()->value_name("INPUT_FILE"), "Input commands file path [default: stdin]")
    ("output_dir,o",opt::value<std::string>()->value_name("OUTPUT_DIR")->default_value(fs::current_path()), "File writters output directory [default: current]")
    ("number,n",    opt::value<size_t>()->value_name("BULK_NUMBER")->default_value(NUM), "Initial bulk commands number")  
  ;
  // clang-format on
  opt::positional_options_description pos_desc;
  pos_desc.add("number", -1);
  opt::variables_map vm;
  opt::store(opt::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
  opt::notify(vm);
  return std::make_tuple(std::move(desc), std::move(vm));
}

inline bool info_requested(const opt::variables_map &vm, const opt::options_description &desc) {
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return true;
  }
  if (vm.count("version")) {
    std::cout << info::info_project_version() << std::endl;
    return true;
  }
  if (vm.count("info")) {
    std::cout << info::info_project_full() << std::endl;
    return true;
  }
  return false;
}

}  // namespace args
