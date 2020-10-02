
#ifndef PHILIPS_HUE_HPP_3CS15LOA
#define PHILIPS_HUE_HPP_3CS15LOA

#include "../../shared/json.hpp"
#include <string>

namespace philips_hue {
std::string config();
void setup(nlohmann::json config);
void dispatch(std::string command);
void free_philips_hue();
} // namespace philips_hue
#endif /* end of include guard: PHILIPS_HUE_HPP_3CS15LOA */
