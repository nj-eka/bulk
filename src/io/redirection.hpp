#pragma once

#include <cerrno>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>

namespace io {

class istream2file_redirect {
  std::istream &_to;
  std::ifstream _from;
  std::streambuf *_saved;

 public:
  istream2file_redirect(std::string_view filename, std::istream &to = std::cin)
      : _to{to}, _from{filename.data(), std::ios::in}, _saved{nullptr} {
    // from.exceptions(from.failbit | from.badbit);
    if (!_from.is_open())
      throw std::system_error(errno ? errno : EIO, std::system_category(),
                              std::format("Failed to open file [{}]", filename));
    _saved = _to.rdbuf(
        _from.rdbuf());  // in this case 'saved' should NOT be initialized in a member initializer of the constructor
  }

  istream2file_redirect(const istream2file_redirect &) = delete;
  void operator=(const istream2file_redirect &) = delete;
  ~istream2file_redirect() {
    if (_saved) _to.rdbuf(_saved);
  }
};

class istream2istream_redirect {
  std::istream &_to;
  std::istream &_from;
  std::streambuf *_saved;

 public:
  istream2istream_redirect(std::istream &from, std::istream &to = std::cin)
      : _to{to}, _from{from}, _saved{_to.rdbuf(_from.rdbuf())} {}

  istream2istream_redirect(const istream2istream_redirect &) = delete;
  void operator=(const istream2istream_redirect &) = delete;
  ~istream2istream_redirect() {
    if (_saved) _to.rdbuf(_saved);
  }
};

}  // namespace io