#include "../../shared/aixlog.hpp"
#include "../../shared/config.hpp"
#include "../../shared/httplib.h"
#include "philips_hue.hpp"

namespace philips_hue {
std::string config() { return "philips_hue"; }
namespace {
// HTTP client that will be initialized by setup_philips_hue
httplib::Client *cli;

std::string username;

std::string ipaddr;
int port;

nlohmann::json commands;

void set_hue_state(std::string username, nlohmann::json states) {
  for (nlohmann::json::iterator it = states.begin(); it != states.end(); ++it) {

    nlohmann::json request = it.value();

    std::string type = request["type"];

    if (type == "lights") {
      std::string hue_id = std::to_string((int)request["id"]);
      std::string hue_state = request["body"].dump();

      // TODO(jonasrsv): Better string formatting
      std::string path = "/api/" + username + "/lights/" + hue_id + "/state";

      LOG(DEBUG) << TAG("philips-hue") << "Setting " << AixLog::Color::GREEN
                 << hue_id << AixLog::Color::NONE << " at "
                 << AixLog::Color::YELLOW << path << AixLog::Color::NONE
                 << " to " << hue_state << std::endl;
      cli->Put(path.c_str(), hue_state, "application/json");
    } else {
      LOG(WARNING) << TAG("philips-hue") << AixLog::Color::RED
                   << "Unknown Request type: " << type << AixLog::Color::NONE
                   << std::endl;
    }
  }
}
} // namespace

void setup(nlohmann::json config) {
  username = config::get_required_config<std::string>(config, "username",
                                                      "philips_hue");
  ipaddr =
      config::get_required_config<std::string>(config, "ipaddr", "philips_hue");
  port = config::get_required_config<int>(config, "port", "philips_hue");
  commands = config::get_required_config<nlohmann::json>(config, "commands",
                                                         "philips_hue");

  LOG(DEBUG) << TAG("philips-hue") << AixLog::Color::YELLOW
             << "Starting http client -- ipaddr: " << ipaddr
             << " port: " << port << AixLog::Color::NONE << std::endl;

  cli = new httplib::Client(ipaddr, port);

  LOG(INFO) << TAG("philips-hue") << AixLog::Color::GREEN << "Success"
            << AixLog::Color::NONE << std::endl;
}

void dispatch(std::string command) {
  if (commands.contains(command)) {
    LOG(INFO) << TAG("philips-hue") << AixLog::Color::YELLOW << "Executing "
              << AixLog::Color::GREEN << command << AixLog::Color::NONE
              << std::endl;

    set_hue_state(username, commands[command]);
  } else {
    LOG(DEBUG) << TAG("philips-hue")
               << "Unknown command: " << AixLog::Color::YELLOW << command
               << AixLog::Color::NONE << std::endl;
  }
}

void free_philips_hue() { free(cli); }

} // namespace philips_hue
