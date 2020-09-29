#include "aixlog.hpp"
#include "json.hpp"
#include <string>

namespace config {

template <typename T>
T get_required_config(nlohmann::json config, std::string name,
                      std::string tag) {
  if (config.contains(name)) {
    return (T)config[name];
  }

  LOG(FATAL) << TAG(tag) << AixLog::Color::RED << "Required field" << name
             << "not in config" << AixLog::Color::NONE << std::endl;
  exit(1);
}

template <typename T>
T get_optional_config(nlohmann::json config, std::string name, std::string tag,
                      T default_) {
  if (config.contains(name)) {
    return (T)config[name];
  }

  LOG(DEBUG) << TAG(tag) << AixLog::Color::CYAN << "Optional field " << name
             << " not in config, using default: " << default_
             << AixLog::Color::NONE << std::endl;

  return default_;
}

nlohmann::json load_json(std::string json_path, std::string tag);

} // namespace config
