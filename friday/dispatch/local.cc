#include "../shared/aixlog.hpp"
#include "local.hpp"
#include <unordered_map>

namespace {
std::unordered_map<std::string, std::string> commands;

void execute_call(std::string call) {
  // TODO some sources say using popen can be bad, look into alternatives
  popen(call.c_str(), "r");

  // Is blocking which is bad
  // system(call.c_str());

  LOG(INFO) << TAG("local") << "Executed " << AixLog::Color::GREEN << call
            << AixLog::Color::NONE << std::endl;
}
} // namespace

namespace local {
std::string config() { return "local"; }

void setup(nlohmann::json config) {
  for (nlohmann::json::iterator it = config.begin(); it != config.end(); ++it) {
    commands[it.key()] = it.value();

    LOG(DEBUG) << TAG("local") << AixLog::Color::YELLOW << it.key()
               << AixLog::Color::NONE << " 'maps to' " << AixLog::Color::GREEN
               << it.value() << AixLog::Color::NONE << std::endl;
  }

  LOG(INFO) << TAG("local") << AixLog::Color::GREEN << "Setup Success"
            << AixLog::Color::NONE << std::endl;
}

void dispatch(std::string command) {
  if (commands.find(command) != commands.end()) {
    LOG(DEBUG) << TAG("local") << AixLog::Color::YELLOW << command
               << AixLog::Color::NONE << " --> " << AixLog::Color::GREEN
               << commands[command] << AixLog::Color::NONE << std::endl;

    execute_call(commands[command]);
  } else {
    LOG(DEBUG) << TAG("local") << "Unknown command: " << AixLog::Color::YELLOW
               << command << AixLog::Color::NONE << std::endl;
  }
}
} // namespace command_launcher
