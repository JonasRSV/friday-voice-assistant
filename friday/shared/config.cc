#include "../setup/friday_options.hpp"
#include "aixlog.hpp"
#include "json.hpp"

namespace config {

nlohmann::json get_launch_config(launch::options *opt, std::string name,
                                 std::string tag) {
  if (opt->configs.find(name) != opt->configs.end()) {
    return opt->configs[name];
  }

  LOG(DEBUG) << TAG(tag) << AixLog::Color::MAGENTA << "Config: '" << name
             << "' not found... Proceeding with assumption that its empty"
             << AixLog::Color::NONE << std::endl;

  return nlohmann::json();
}

} // namespace config
