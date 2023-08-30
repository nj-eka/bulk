#pragma once

#include <functional>
#include <memory>

namespace utils {

/**
 * @brief defer wrapper
 *
 * @code {.cpp}
 * defer d0 { [&]{ ...; } };
 * @endcode
 *
 */
class defer {
  std::function<void()> _f{[]() {}};
  defer(){};

 public:
  explicit defer(std::function<void()> f) : _f{f} {}
  ~defer() { _f(); }
};

// /// version with shared_ptr
//  //The guard's deleter always executes on return/exit
//  //The shared_ptr is null but not empty
//  std::shared_ptr<void>
//  guard(nullptr, [&](void*){ ...; });

// @ https://eax.me/cpp-smart-pointers/
// g++ custom-deleter.cpp -o custom-deleter */
// template <typename T>
// using auto_cleanup = std::unique_ptr<T, std::function<void(T*)>>;
// static char dummy[] = "";
// // clang-format off
// define _DEFER_CAT_(a,b) a##b
// define _DEFER_NAME_(a,b) _DEFER_CAT_(a,b)
// define defer(...)
//   auto _DEFER_NAME_(_defer_,__LINE__) =
//     auto_cleanup<char>(dummy, [&](char*) { __VA_ARGS__; }); //!< ошибка: expected primary-expression before «)» ...
// // clang-format on
// // usage:
// //     auto_cleanup<FILE> f(
// //         fopen("test.txt", "w"),
// //         [](FILE* f) { fclose(f); }
// //     );
// //     defer( std::cout << "Bye #1" << std::endl );

}  // namespace utils