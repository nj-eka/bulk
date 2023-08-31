#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>

#include "../bulk/bulk_event.hpp"
#include "../core/subscriber.hpp"

namespace logs {

namespace fs = std::filesystem;

class FileWriter final : public core::SubscriberBase<bulk::BulkEvent>, public std::enable_shared_from_this<FileWriter> {
 public:
  [[nodiscard]] static auto create(std::string_view output_directory, std::string_view name = "") {
    return std::shared_ptr<FileWriter>(new FileWriter(output_directory, name));
  }

  core::SubscriptionStatus getSubscriptionStatus() noexcept override { return _subs_state; }

  void setSubscriptionStatus(core::SubscriptionStatus state) noexcept { _subs_state = state; }

  core::SubscriptionStatus onEvent(bulk::BulkEvent const& event) override {
    if (core::SubscriptionStatus::READY == getSubscriptionStatus()) {
      auto ts = std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(event.time_stamp.time_since_epoch()).count());
      std::ofstream fw(std::format("{}/{}_bulk{}_{}.log", _output_dir.c_str(), _local_name, ++_bulk_counter, ts));
      if (!fw.is_open()) {
        setSubscriptionStatus(core::SubscriptionStatus::CANCEL);
      } else {
        bulk::print(event, fw);
        fw.close();
      }
    }
    return getSubscriptionStatus();
  }

 private:
  fs::path const _output_dir;
  std::string const _local_name;
  std::string const _name;
  size_t _bulk_counter = 0;
  core::SubscriptionStatus _subs_state;

  FileWriter(std::string_view output_directory, std::string_view local_name)
      : _output_dir{std::filesystem::canonical(output_directory)},
        _local_name{local_name.size() ? local_name : std::to_string(_instanceNum())},
        _name{std::format("{}.{}", _output_dir.c_str(), _local_name)},
        _subs_state{core::SubscriptionStatus::READY} {}

  std::string getSubscriberName() const noexcept override { return _name; }

  size_t _instanceNum() const {
    static size_t _counter = 0;
    return ++_counter;
  }
};

}  // namespace logs