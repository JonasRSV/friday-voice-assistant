#include "../../testing/catch.hpp"
#include "philips_hue.hpp"
#include <unistd.h>

TEST_CASE("Load config", "Philips Hue") {
  // Attempt to load config
  philips_hue::setup_philips_hue("configs/philips-hue/philips_hue_config.json");
}

TEST_CASE("Turn on and off lights", "Philips Hue") {
  philips_hue::setup_philips_hue("configs/philips-hue/philips_hue_config.json");
  philips_hue::dispatch("lights_on");

  // wait a little
  sleep(2);

  philips_hue::dispatch("lights_off");
}
