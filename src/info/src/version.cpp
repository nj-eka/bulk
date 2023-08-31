#include <info/version.h>

#include <sstream>

std::string info::info_project_full() {
  std::stringstream ss;
  ss << "author: " << info_project_author() << std::endl;
  ss << "url: " << info_project_repository_url() << std::endl;
  ss << "version: " << info_project_version() << " (revision:" << info_project_revision() << ")" << std::endl;

  ss << "compiler: " << info_compiler_name() << " " << info_compiler_version() << std::endl;
  ss << "platform: " << info_host_system() << std::endl;
  ss << "using:\n"
     << "\tBoost version " << info_boost_version() << "\n\t\tlibraries: " << info_boost_libraries() << std::endl;

  ss << "branch: " << info_project_repository_branch() << std::endl;
  ss << "rev date: " << info_project_revision_datetime() << std::endl;
  ss << "rev hist: " << info_project_revision_history() << std::endl;
  return ss.str();
}