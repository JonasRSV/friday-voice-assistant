#include "../../testing/catch.hpp"
#include "../../shared/config.hpp"
#include "philips_hue.hpp"
#include <unistd.h>

TEST_CASE("Turn on and off lights", "Philips Hue") {

  nlohmann::json config = config::load_json("configs/philips_hue.json", "philips_hue");
  philips_hue::setup(config);
  philips_hue::dispatch("tänd ljuset");

  // wait a little
  sleep(6);

  philips_hue::dispatch("släck ljuset");


}
