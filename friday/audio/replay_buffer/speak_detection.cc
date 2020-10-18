#include "../../shared/config.hpp"
#include "speak_detection.hpp"
#include <chrono>

namespace speak_detection {
std::string config() { return "speak_detection"; }

namespace {
// frame_size of the model
double frame_size;

// The mean energy is a measure of the average energy.
double mean_energy;

// How quickly the mean energy is increased.
double positive_energy_transfer_rate;

// How quickly the mean energy is decreased.
double negative_energy_transfer_rate;

// How far above the energy has to be for the inference to be called
double deviation_energy;

// Current energy
double energy = 0;

// ms to wait before predicting after first speaker identification
int time_delay;
} // namespace

void initialize(int16_t *buffer, size_t buffer_size) {
  frame_size = (double)buffer_size;

  for (size_t i = 0; i < buffer_size; i++) {
    energy += sqrt((double)buffer[i] * buffer[i]);
  }
}

void slide(int16_t dropped_sample, int16_t added_sample) {
  energy -= sqrt((double)(dropped_sample * dropped_sample));
  energy += sqrt((double)(added_sample * added_sample));
}

void setup(nlohmann::json config) {
  positive_energy_transfer_rate = config::get_optional_config<double>(
      config, "positive_energy_transfer_rate", /*tag=*/"speak_detection",
      /*default=*/0.1);
  negative_energy_transfer_rate = config::get_optional_config<double>(
      config, "negative_energy_transfer_rate", /*tag=*/"speak_detection",
      /*default=*/0.4);
  mean_energy = config::get_optional_config<double>(
      config, "mean_energy", /*tag=*/"speak_detection", /*default=*/2000);
  deviation_energy = config::get_optional_config<double>(
      config, "deviation_energy", /*tag=*/"speak_detection", /*default=*/30);

  time_delay = config::get_optional_config<int>(
      config, "time_delay ms", /*tag=*/"speak_detection", /*default=*/1000);
}

auto timestamp = std::chrono::steady_clock::now();

bool has_speaker() {
  double signal_energy = energy / frame_size;

  LOG(INFO) << TAG("speak_detection") << AixLog::Color::YELLOW
             << "energy " << energy << " mean energy: " << mean_energy << " threshold "
             << mean_energy + deviation_energy << " energy " << signal_energy
             << AixLog::Color::NONE << std::endl;

  // Update the mean energy
  if (signal_energy > mean_energy)
    mean_energy = mean_energy * (1 - positive_energy_transfer_rate) +
                  signal_energy * positive_energy_transfer_rate;
  else
    mean_energy = mean_energy * (1 - negative_energy_transfer_rate) +
                  signal_energy * negative_energy_transfer_rate;

  auto current_time = std::chrono::steady_clock::now();
  if (signal_energy > mean_energy + deviation_energy) {
      // If enough time has passed, e.g don't want to predict on first syllable on utterance
    if (std::chrono::duration_cast<std::chrono::milliseconds>(current_time - timestamp).count() > time_delay)
      return true;

  } else {
    timestamp = current_time;
  }

  return false;
}

void clear() {}

} // namespace speak_detection
