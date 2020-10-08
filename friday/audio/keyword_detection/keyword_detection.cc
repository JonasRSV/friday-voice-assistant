#include "../../shared/aixlog.hpp"
#include "../../shared/config.hpp"
#include "../../shared/json.hpp"
#include "../../shared/print_utils.hpp"
#include "../playback/playback.hpp"
#include "../replay_buffer/replay_buffer.hpp"
#include "goldfish/goldfish.hpp"
#include "keyword_detection.hpp"
#include <string>
#include <unistd.h>

namespace keyword_detection {

#define SECONDS(x) (long long)(x * 1000000)

nlohmann::json index_to_name;

float certainty_barrier;
size_t prediction_dim;
float *probability_buffer;

std::string config() { return "keyword_detection"; }

namespace {

void reset_buffer() {
  for (size_t i = 0; i < prediction_dim; i++)
    probability_buffer[i] = 0.0;
}

size_t argmax(float *buffer) {

  size_t largest_probability_index = 0;
  float largest_probability = buffer[0];

  for (size_t i = 0; i < prediction_dim; i++) {
    if (buffer[i] > largest_probability) {
      largest_probability = buffer[i];
      largest_probability_index = i;
    }
  }

  return largest_probability_index;
}

float max(float *buffer) {

  float largest_probability = buffer[0];

  for (size_t i = 0; i < prediction_dim; i++) {
    if (buffer[i] > largest_probability) {
      largest_probability = buffer[i];
    }
  }

  return largest_probability;
}

} // namespace

void setup(nlohmann::json config) {
  LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::GREEN
             << "Setting up keyword detection" << AixLog::Color::NONE
             << std::endl;

  std::string label_map_path = config::get_required_config<std::string>(
      config, "label_map_path", /*tag=*/"keyword_detection");

  nlohmann::json name_to_index =
      config::load_json(label_map_path, /*tag=*/"keyword_detection");

  // Reverse label map to use as lookup in predictions
  for (auto it = name_to_index.begin(); it != name_to_index.end(); it++)
    index_to_name[std::to_string((int)it.value())] = it.key();

  certainty_barrier =
      config::get_optional_config<float>(config, "certainty_barrier",
                                         /*tag=*/"keyword_detection",
                                         /*default=*/0.5);

  prediction_dim = goldfish::prediction_dim();
  probability_buffer = (float *)calloc(prediction_dim, sizeof(float));

  LOG(INFO) << TAG("keyword_detection") << AixLog::Color::GREEN
            << "Setup keyword detection -- label_map_path: " << label_map_path
            << " -- certainty_barrier: " << certainty_barrier
            << AixLog::Color::NONE << std::endl;
}

std::string prediction() {
  reset_buffer();

  size_t frame_size = replay_buffer::frame_size();

  // prediction returned by model
  goldfish::model_prediction pred;

  // Index of prediction (argmax)
  int prediction;

  while (true) {

    // Blocks until we get a new frame
    int16_t *predict_frame = replay_buffer::next_sample();

    if (predict_frame != nullptr) {

      pred = goldfish::predict(predict_frame, frame_size);
      prediction = argmax(pred.probabilities.data());

      LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                 << "probabilities: " << pred.probabilities
                 << AixLog::Color::NONE << std::endl;

      if (prediction != 0) {
        float certainty = max(pred.probabilities.data());

        LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                   << "buffer_choice: " << prediction
                   << " name: " << index_to_name[std::to_string(prediction)]
                   << " certainty: " << certainty << AixLog::Color::NONE
                   << std::endl;

        // If we are certain enough we make a prediction
        if (max(pred.probabilities.data()) > certainty_barrier) {
          // std::cout << "Picked " << std::endl;
          // playback::play_audio_frame(predict_frame, 16000, 8000);
          // usleep(SECONDS(5.0));

          return index_to_name[std::to_string(prediction)];
        } else
          LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::CYAN
                     << "best guess is "
                     << index_to_name[std::to_string(prediction)]
                     << " certainty: " << certainty << AixLog::Color::NONE
                     << std::endl;
      }
    }
  }
}

void clear() { delete[] probability_buffer; }

} // namespace keyword_detection
