
#ifndef PHILIPS_HUE_HPP_3CS15LOA
#define PHILIPS_HUE_HPP_3CS15LOA

#include <string>

namespace philips_hue {
void setup_philips_hue(std::string path_to_hue_config_file);
void dispatch(std::string command);
void free_philips_hue();
} // namespace philips_hue
#endif /* end of include guard: PHILIPS_HUE_HPP_3CS15LOA */
