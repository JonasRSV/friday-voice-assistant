#include "../../../shared/json.hpp"
#include "../../../testing/catch.hpp"
#include "goldfish.hpp"
#include "iostream"

int16_t *get_mock_audio(size_t size) {
  int16_t *audio = (int16_t *)malloc(sizeof(int16_t) * size);

  for (size_t i = 0; i < size; i++) {
    audio[i] = (rand() * 4 + i) % 16000;
  }

  return audio;
}

TEST_CASE("Load model", "Goldfish") {
  nlohmann::json config;
  config["export_dir"] =
      "friday/audio/keyword_detection/goldfish/models/saved_models/test_model";
  goldfish::setup(config);
}

TEST_CASE("Make prediction", "Goldfish") {
  nlohmann::json config;
  config["export_dir"] =
      "friday/audio/keyword_detection/goldfish/models/saved_models/test_model";
  goldfish::setup(config);

  int16_t *audio = get_mock_audio(8000);

  goldfish::model_prediction pred = goldfish::predict(/*audio=*/audio,
                                                      /*size=*/8000);

  REQUIRE(pred.index == 9);
}
