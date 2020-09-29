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

float sleep_between_predictions;
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
  float predictions_per_second =
      config::get_optional_config<float>(config, "predictions_per_second",
                                         /*tag=*/"keyword_detection",
                                         /*default=*/3.0);

  sleep_between_predictions = 1 / predictions_per_second;

  prediction_dim = goldfish::prediction_dim();
  probability_buffer = (float *)calloc(prediction_dim, sizeof(float));

  LOG(INFO) << TAG("keyword_detection") << AixLog::Color::GREEN
            << "Setup keyword detection\nlabel_map_path: " << label_map_path
            << "\ncertainty_barrier: " << certainty_barrier
            << "\npredictions_per_second: " << predictions_per_second
            << AixLog::Color::NONE << std::endl;
}

std::string prediction() {

  size_t frame_size = replay_buffer::frame_size();

  // If a prediction has been made and if accumulation buffer is active
  bool predict_flag = false, buffer_active_flag = false;

  // The number of predictions in accumulation buffer
  float num_accum_predictions = 0.0;

  // prediction returned by model
  goldfish::model_prediction pred;

  // Index of prediction (argmax)
  int prediction;
  while (true) {
    usleep(SECONDS(sleep_between_predictions));
    int16_t *predict_frame = replay_buffer::next_sample();

    if (predict_frame != nullptr) {
      pred = goldfish::predict(predict_frame, frame_size);
      predict_flag = true;
    } else {
      predict_flag = false;
    }

    // The heuristic:
    // If a prediction is made we will start accumulating predictions until
    // a silence is encountered

    // Start with checking if a prediction was made, if so, get the most likely
    // class
    if (predict_flag) {
      prediction = argmax(pred.probabilities.data());

      LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                 << "predicted: " << prediction << AixLog::Color::NONE
                 << std::endl;

      // If the most likely class is not silence, then we start accumulating
      // IMPORTANT: Here we assume that 0 is always silence
      if (prediction != 0) {

        buffer_active_flag = true;

        // add probabilities to our buffer
        num_accum_predictions += 1.0;
        add(pred.probabilities.data());

        // Now we continue because everything after this code is assumed to be
        // silence
        continue;
      }
    }

    // Here some kind of silence has occured, either predicted or just low
    // energy. If our buffer is active we check how certain we are about the
    // most probable class and if we are certain enough we return it
    if (buffer_active_flag) {

      // get certainty of our most likely class
      size_t most_likely = argmax(probability_buffer);
      float certainty = max(probability_buffer) / num_accum_predictions;

      LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                 << "buffer_choice: " << most_likely
                 << " name: " << index_to_name[std::to_string(most_likely)]
                 << " certainty: " << certainty << AixLog::Color::NONE
                 << std::endl;

      // If we are certain enough we make a prediction
      if (max(probability_buffer) / num_accum_predictions > certainty_barrier)
        return index_to_name[std::to_string(most_likely)];

      LOG(DEBUG) << TAG("keyword_detection") << AixLog::Color::YELLOW
                 << "resetting buffer" << AixLog::Color::NONE << std::endl;

      // Otherwise we reset the buffer and start over
      buffer_active_flag = false;
      reset_buffer();
    }
  }
}

void clear() { delete[] probability_buffer; }

} // namespace keyword_detection
