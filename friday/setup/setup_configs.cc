#include "../shared/aixlog.hpp"
#include "../shared/config.hpp"
#include "dirent.h"
#include "setup_configs.hpp"

namespace launch {
namespace {
bool is_json_file(std::string filename) {
  return filename.substr(filename.find_last_of(".") + 1) == "json";
}

std::string file_without_extension(std::string filename) {
  return filename.substr(0, filename.find_last_of("."));
}

std::string get_full_path(std::string config_directory, std::string file_name) {
  return config_directory + "/" + file_name;
}

} // namespace

std::map<std::string, nlohmann::json>
load_configs(std::string config_directory) {

  struct dirent *entry;
  DIR *dir = opendir(config_directory.c_str());

  printf("config_directory %s ", config_directory.c_str());

  if (dir == NULL) {
    LOG(FATAL) << "Could not open config directory: " << config_directory
               << " does it exist?" << std::endl;
    exit(1);
  }

  LOG(DEBUG) << AixLog::Tag("launch") << "Loading configs from "
             << AixLog::Color::CYAN << config_directory << AixLog::Color::NONE
             << std::endl;

  std::map<std::string, nlohmann::json> configs;
  while ((entry = readdir(dir)) != NULL) {
    std::string filename(entry->d_name);

    if (is_json_file(filename)) {

      std::string config_name = file_without_extension(filename);
      std::string full_path = get_full_path(config_directory, filename);

      LOG(DEBUG) << AixLog::Tag("launch") << "Loading " << AixLog::Color::GREEN
                 << full_path << AixLog::Color::NONE << std::endl;

      configs[config_name] = config::load_json(full_path, "launch");
    } else {
      LOG(DEBUG) << AixLog::Tag("launch") << "Ignoring " << AixLog::Color::RED
                 << filename << AixLog::Color::NONE << std::endl;
    }
  }

  closedir(dir);
  return configs;
}

} // namespace launch
