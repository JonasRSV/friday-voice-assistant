#include "aixlog.hpp"
#include "json.hpp"
#include "json_utils.hpp"

namespace json_utils {
nlohmann::json load_json(std::string json_path, std::string tag) {
  std::ifstream i(json_path);
  if (!i.good()) {
    LOG(FATAL) << TAG(tag) << AixLog::Color::RED << "Failed to load "
               << json_path << AixLog::Color::NONE << std::endl;
    exit(1);
  }
  nlohmann::json config;

  // Reads json file
  i >> config;

  return config;
}

} // namespace json_utils
