#include "../../testing/catch.hpp"
#include "iostream"
#include "replay_buffer.hpp"

int16_t *audio;

int16_t *get_mock_audio() {

  for (size_t i = 0; i < 8000; i++) {
    audio[i] = (rand() * 4 + i) % 16000;
  }

  return audio;
}

TEST_CASE("Run Replay Buffer Simulation", "replay_buffer") {
  int16_t *audio = (int16_t *)malloc(sizeof(int16_t) * 8000);

  nlohmann::json config;
  config["buffer_size"] = 16000;
  config["frame_size"] = 8000;
  config["max_predict_delay"] = 4000;
  replay_buffer::setup(config);


  for (int i = 0; i < 10; i++) {
    replay_buffer::add(audio, 8000);

    for (int j = 0; j < 5; j++) {
      replay_buffer::next_sample();
    }
  }
}
