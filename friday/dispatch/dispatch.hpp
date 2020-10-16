
#ifndef DISPATCH_HPP_DQ4FKVRT
#define DISPATCH_HPP_DQ4FKVRT

#include "../shared/json.hpp"

namespace dispatch {

std::string config();
void dispatch(std::string);
void setup(nlohmann::json);
void clear();

} // namespace dispatch

#endif /* end of include guard: DISPATCH_HPP_DQ4FKVRT */
