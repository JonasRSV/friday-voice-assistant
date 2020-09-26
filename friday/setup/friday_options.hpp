#ifndef FRIDAY_OPTIONS_HPP_ACUJUPZW
#define FRIDAY_OPTIONS_HPP_ACUJUPZW

#include "../shared/json.hpp"
#include <map>
#include <string>

namespace launch {

struct options {
  std::map<std::string, nlohmann::json> configs;
};

options *parse_options(int argc, const char *argv[]);

void free_options(options *opts);
} // namespace launch

#endif /* end of include guard: FRIDAY_OPTIONS_HPP_ACUJUPZW */
