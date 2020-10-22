#include "../shared/aixlog.hpp"
#include "../third_party/philips-hue/philips_hue.hpp"
#include "dispatch.hpp"
#include "local.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>

namespace dispatch {
std::string config() { return "dispatch"; }

namespace {
void dispatch_all(std::string keyword) {
  LOG(DEBUG) << TAG("dispatch") << AixLog::Color::YELLOW
             << "Dispatching to philips_hue: " << keyword << AixLog::Color::NONE
             << std::endl;
  philips_hue::dispatch(keyword);
  LOG(DEBUG) << TAG("dispatch") << AixLog::Color::YELLOW
             << "Dispatching to local: " << keyword << AixLog::Color::NONE
             << std::endl;
  local::dispatch(keyword);
  LOG(INFO) << TAG("dispatch") << AixLog::Color::MAGENTA << "Dispatching '" << keyword << "' complete"
             << AixLog::Color::NONE << std::endl;
}
} // namespace

void setup(nlohmann::json config) {

  LOG(INFO) << TAG("dispatch") << AixLog::Color::GREEN << "Setup success"
            << AixLog::Color::NONE << std::endl;
}

void dispatch(std::string keyword) {
  std::thread dispatch_thread(dispatch_all, keyword);
  dispatch_thread.detach();
}

void clear() {}
} // namespace dispatch
