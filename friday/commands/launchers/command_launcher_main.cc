#include "../../shared/aixlog.hpp"
#include "../../shared/config_loader.hpp"
#include "command_launcher.hpp"
#include <stdio.h>
#include <unordered_map>

namespace {
std::unordered_map<std::string, std::string> commands;

void execute_call(std::string call) {
  LOG(DEBUG) << TAG("command-launcher dispatch") << AixLog::Color::YELLOW
             << "Attempting to execute " << AixLog::Color::NONE
             << AixLog::Color::GREEN << call << AixLog::Color::NONE
             << std::endl;

  // TODO some sources say using popen can be bad, look into alternatives
  popen(call.c_str(), "r");

  // Is blocking which is bad
  // system(call.c_str());

  LOG(INFO) << TAG("command-launcher dispatch") << "Executed "
            << AixLog::Color::GREEN << call << AixLog::Color::NONE << std::endl;
}
} // namespace

namespace command_launcher {
void setup_command_launcher(std::string path_to_commands) {
  LOG(DEBUG) << TAG("command-launcher setup")
             << "Attempting to load commands from " << AixLog::Color::YELLOW
             << path_to_commands << AixLog::Color::NONE << std::endl;

  nlohmann::json config = shared::load_config(path_to_commands);

  for (nlohmann::json::iterator it = config.begin(); it != config.end(); ++it) {
    commands[it.key()] = it.value();

    LOG(DEBUG) << TAG("command-launcher setup") << AixLog::Color::YELLOW
               << it.key() << AixLog::Color::NONE << " 'maps to' "
               << AixLog::Color::GREEN << it.value() << AixLog::Color::NONE
               << std::endl;
  }

  LOG(INFO) << TAG("command-launcher setup") << AixLog::Color::GREEN
            << "Success" << AixLog::Color::NONE << std::endl;
}

void dispatch(std::string command) {
  if (commands.find(command) != commands.end()) {
    LOG(DEBUG) << TAG("command-launcher dispatch") << AixLog::Color::YELLOW
               << command << AixLog::Color::NONE << " --> "
               << AixLog::Color::GREEN << commands[command]
               << AixLog::Color::NONE << std::endl;

    execute_call(commands[command]);
  } else {
    LOG(DEBUG) << TAG("command-launcher dispatch")
               << "Unknown command: " << AixLog::Color::YELLOW << command
               << AixLog::Color::NONE << std::endl;
  }
}
} // namespace command_launcher
