#include "../../shared/aixlog.hpp"
#include "../../shared/config_loader.hpp"
#include "../shared/httplib.h"
#include "philips_hue.hpp"

namespace {
// HTTP client that will be initialized by setup_philips_hue
httplib::Client *cli;
nlohmann::json config;

void set_hue_state(std::string username, nlohmann::json states) {
  for (nlohmann::json::iterator it = states.begin(); it != states.end(); ++it) {
    std::string hue_id = it.key();
    std::string hue_state = it.value().dump();

    // TODO(jonasrsv): Better string formatting
    std::string path = "/api/" + username + "/lights/" + hue_id + "/state";

    LOG(DEBUG) << TAG("philips hue set") << "Setting " << AixLog::Color::GREEN
               << hue_id << AixLog::Color::NONE << " at "
               << AixLog::Color::YELLOW << path << AixLog::Color::NONE << " to "
               << hue_state << std::endl;

    cli->Put(path.c_str(), hue_state, "application/json");
  }
}
} // namespace

namespace philips_hue {

void setup_philips_hue(std::string path_to_hue_config_file) {
  LOG(DEBUG) << TAG("philips hue setup") << "Attempting to load commands from "
             << AixLog::Color::YELLOW << path_to_hue_config_file
             << AixLog::Color::NONE << std::endl;

  config = shared::load_config(path_to_hue_config_file);

  LOG(DEBUG) << TAG("philips hue setup") << AixLog::Color::YELLOW
             << "Starting http client -- ipaddr: " << config["ipaddr"]
             << " port: " << config["port"] << AixLog::Color::NONE << std::endl;

  cli = new httplib::Client(config["ipaddr"], config["port"]);

  LOG(INFO) << TAG("philips hue setup") << AixLog::Color::GREEN << "Success"
            << AixLog::Color::NONE << std::endl;
}

void dispatch(std::string command) {
  if (config["commands"].contains(command)) {
    LOG(INFO) << TAG("philips hue dispatch") << AixLog::Color::YELLOW
              << "Executing " << AixLog::Color::GREEN << command
              << AixLog::Color::NONE << std::endl;

    set_hue_state(config["username"], config["commands"][command]);
  } else {
    LOG(DEBUG) << TAG("philips hue dispatch")
               << "Unknown command: " << AixLog::Color::YELLOW << command
               << AixLog::Color::NONE << std::endl;
  }
}

void free_philips_hue() { free(cli); }

} // namespace philips_hue
