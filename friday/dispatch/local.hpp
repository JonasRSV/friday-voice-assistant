#ifndef LOCAL_HPP_ISXL3OYK
#define LOCAL_HPP_ISXL3OYK

#include "../shared/json.hpp"
#include <string>

namespace local {
std::string config();
void setup(nlohmann::json config);
void dispatch(std::string command);
void clear();
} // namespace local

#endif /* end of include guard: LOCAL_HPP_ISXL3OYK */
