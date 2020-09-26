
#ifndef SETUP_CONFIGS_HPP_EDMVGY3X
#define SETUP_CONFIGS_HPP_EDMVGY3X

#include "../shared/json.hpp"
#include <map>
#include <string>

namespace launch {
std::map<std::string, nlohmann::json> load_configs(std::string config_dir);

}

#endif /* end of include guard: SETUP_CONFIGS_HPP_EDMVGY3X */
