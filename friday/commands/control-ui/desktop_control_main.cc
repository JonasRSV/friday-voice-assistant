#include "../../shared/aixlog.hpp"
#include "../../shared/config_loader.hpp"
#include "desktop_control.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>

namespace {
std::string previous_workspace = "0";
std::string previous_window = "";

std::string switch_workspace_command = "";

std::string get_current_workspace() {
  std::string cmd = "wmctrl -d | grep '*' | head -c 1";

  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);

  if (!pipe)
    LOG(ERROR) << TAG("desktop control") << AixLog::Color::RED
               << "Failed to open pipe" << AixLog::Color::NONE << std::endl;

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result.c_str();
}

void switch_to_previous_workspace() {
  std::string tmp_previous_workspace = get_current_workspace();

  std::string command = "wmctrl -s " + previous_workspace;

  LOG(DEBUG) << TAG("desktop control") << AixLog::Color::YELLOW
             << "Switch desktop command: " << command << AixLog::Color::NONE
             << std::endl;

  system(command.c_str());

  previous_workspace = tmp_previous_workspace;
}

} // namespace

namespace desktop_control {

void setup_desktop(std::string path_to_desktop_control_config) {

  LOG(DEBUG) << TAG("desktop control setup") << "Trying to load "
             << AixLog::Color::YELLOW << path_to_desktop_control_config
             << AixLog::Color::NONE << std::endl;

  nlohmann::json config = shared::load_config(path_to_desktop_control_config);

  switch_workspace_command = config["desktop_switch"];
  previous_workspace = get_current_workspace();

  LOG(INFO) << TAG("desktop control setup") << AixLog::Color::GREEN << "Success"
            << AixLog::Color::NONE << std::endl;
}

void dispatch(std::string command) {
  if (command == switch_workspace_command) {
    LOG(INFO) << TAG("desktop control dispatch") << AixLog::Color::GREEN
              << "Switching desktop" << AixLog::Color::NONE << std::endl;
    switch_to_previous_workspace();
  } else {
    LOG(DEBUG) << TAG("desktop control dispatch") << "Unknown control "
               << AixLog::Color::YELLOW << command << AixLog::Color::NONE
               << std::endl;
  }
}

} // namespace desktop_control
