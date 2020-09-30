#include "../../testing/catch.hpp"
#include "../recording/recording.hpp"
#include "iostream"
#include "playback.hpp"
#include "unistd.h"

#define SECOND(x) (long long)(x * 1000000)

TEST_CASE("Run Playback", "playback") {
  nlohmann::json audio_config;
  audio_config["device"] = "default";
  audio_config["sample_rate"] = 8000;
  audio_config["frame_length"] = 80000;
  recording::setup(audio_config);

  nlohmann::json playback_config;
  playback::setup(playback_config);


  int16_t *audio_frame = recording::get_next_audio_frame();

  playback::play_audio_frame(audio_frame, recording::frame_size(),
                             recording::sample_rate());
}
