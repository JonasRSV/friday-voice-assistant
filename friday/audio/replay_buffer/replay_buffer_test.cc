#include "../../testing/catch.hpp"
#include "../recording/recording.hpp"
#include "iostream"
#include "replay_buffer.hpp"
#include "unistd.h"

#define SECOND(x) (long long)(x * 1000000)

TEST_CASE("Run Replay Buffer Simulation", "replay_buffer") {
  nlohmann::json audio_config;
  audio_config["device"] = "default";
  audio_config["sample_rate"] = 8000;
  audio_config["frame_length"] = 8000;
  recording::setup(audio_config);

  nlohmann::json config;
  config["buffer_size"] = 80000;
  config["frame_size"] = 8000;
  config["max_predict_delay"] = 4000;
  replay_buffer::setup(config);

  for (int i = 0; i < 10; i++) {
    replay_buffer::next_sample();

    usleep(SECOND(0.5));
  }
}
