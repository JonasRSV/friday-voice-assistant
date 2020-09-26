#ifndef DESKTOP_CONTROL_HPP_OPOIUJWH
#define DESKTOP_CONTROL_HPP_OPOIUJWH

#include <string>

namespace desktop_control {

void setup_desktop(std::string path_to_desktop_control_config);
void dispatch(std::string command);

} // namespace desktop_control
#endif /* end of include guard: DESKTOP_CONTROL_HPP_OPOIUJWH */
