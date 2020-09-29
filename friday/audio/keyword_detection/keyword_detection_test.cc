#include "../../testing/catch.hpp"
#include "../recording/recording.hpp"
#include "../replay_buffer/replay_buffer.hpp"
#include "goldfish/goldfish.hpp"
#include "iostream"
#include "keyword_detection.hpp"
#include "unistd.h"

#define SECOND(x) (long long)(x * 1000000)

TEST_CASE("Run Predict Simulation", "keyword_detection") {
  nlohmann::json audio_config;
  audio_config["device"] = "default";
  audio_config["sample_rate"] = 8000;
  audio_config["frame_length"] = 8000;
  recording::setup(audio_config);

  nlohmann::json replay_config;
  replay_config["buffer_size"] = 80000;
  replay_config["frame_size"] = 8000;
  replay_config["max_predict_delay"] = 4000;
  replay_buffer::setup(replay_config);

  nlohmann::json model_config;
  model_config["export_dir"] =
      "friday/audio/keyword_detection/goldfish/models/saved_models/test_model";
  goldfish::setup(model_config);

  nlohmann::json config;
  config["label_map_path"] = "friday/audio/keyword_detection/goldfish/configs/"
                             "google_speech_commands_label_map.json";
  keyword_detection::setup(config);

  std::string prediction = keyword_detection::prediction();

  std::cout << "prediction " << prediction << std::endl;
}
