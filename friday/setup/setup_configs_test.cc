#include "../testing/catch.hpp"
#include "setup_configs.hpp"
#include "iostream"


TEST_CASE("Load configs", "Launcher") {
  // Attempt to load config
  launch::load_configs("configs");
}
