#include "../../shared/aixlog.hpp"
#include "../../shared/config.hpp"
#include "../../shared/json.hpp"
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
      largest_probability = probability_buffer[i];
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

void add(float *prediction) {
  for (size_t i = 0; i < prediction_dim; i++)
    probability_buffer[i] += prediction[i];
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

  // If a prediction has been made and if accumulation buffer is active
  bool predict_flag = false, is_unknown = true, buffer_active = false;

  // The number of predictions in accumulation buffer
  float num_accum_predictions = 0.0;

  // prediction returned by model
  goldfish::model_prediction pred;

  // Index of prediction (argmax)
  int prediction;
  while (true) {
    is_unknown = true;

    // Blocks until we get a new frame
    int16_t *predict_frame = replay_buffer::next_sample();

    if (predict_frame != nullptr) {
      pred = goldfish::predict(predict_frame, frame_size);
      predict_flag = true;
    } else {
      predict_flag = false;
    }

    // The heuristic:
    // If a prediction is made of a known class we will start accumulating until
    // 1 of 2 thing happends
    // 1. The prediction threshhold is reached and we return prediction.
    // 2. A silence or unknown prediction is encountered and we reset

    // Start with checking if a prediction was made, if so, get the most likely
    // class
    if (predict_flag) {
      prediction = argmax(pred.probabilities.data());

      LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                 << "predicted: " << prediction << AixLog::Color::NONE
                 << std::endl;

      // If the most likely class is not unknown, then we accumulate
      // IMPORTANT: Here we assume that 0 is always silence
      if (prediction != 0) {
        // add probabilities to our buffer
        num_accum_predictions += 1.0;
        add(pred.probabilities.data());

        is_unknown = false;
        buffer_active = true;
      }
    }

    // If the prediction is of a known class
    if (!is_unknown) {

      if (num_accum_predictions > 1.0) {
        // get certainty of our most likely class
        float certainty = max(probability_buffer) / num_accum_predictions;

        LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                   << "buffer_choice: " << prediction
                   << " name: " << index_to_name[std::to_string(prediction)]
                   << " certainty: " << certainty << AixLog::Color::NONE
                   << std::endl;

        // If we are certain enough we make a prediction
        if (max(probability_buffer) / num_accum_predictions > certainty_barrier)
          return index_to_name[std::to_string(prediction)];
        else
          LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::CYAN
                     << "best guess is "
                     << index_to_name[std::to_string(prediction)]
                     << " certainty: " << certainty << AixLog::Color::NONE
                     << std::endl;
      }

    } else if (buffer_active) {
      // Is a unknown class, we will reset.
      LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                 << "resetting buffer" << AixLog::Color::NONE << std::endl;

      // Otherwise we reset the buffer and start over
      num_accum_predictions = 0.0;
      reset_buffer();
      buffer_active = false;
    }
  }
}

void clear() { delete[] probability_buffer; }

} // namespace keyword_detection
