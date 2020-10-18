#include "../../testing/catch.hpp"
#include "../recording/recording.hpp"
#include "iostream"
#include "playback.hpp"
#include "unistd.h"

#define SECOND(x) (long long)(x * 1000000)

TEST_CASE("Run Playback", "playback") {
  nlohmann::json audio_config;
  audio_config["device"] = "sysdefault:CARD=Device";
  audio_config["sample_rate"] = 8000;
  audio_config["frame_length"] = 80000;
  recording::setup(audio_config);

  nlohmann::json playback_config;
  playback::setup(playback_config);

  int16_t *audio_frame = recording::get_next_audio_frame();


  for (int i = 0; i < audio_config["frame_length"]; i++) {
     //audio_frame[i] = hf.filter(lf.filter(audio_frame[i] * 20));

     //int amplified = lf.filter(audio_frame[i]);
     int amplified = audio_frame[i];
     amplified = amplified * 2;
     amplified = std::min(amplified, 32000);
     amplified = std::max(amplified, -32000);

    
     audio_frame[i] = amplified;
     std::cout << audio_frame[i] << "        ";
  }
  std::cout << std::endl;

  playback::play_audio_frame(audio_frame, recording::frame_size(),
                             recording::sample_rate());
}
