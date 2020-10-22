#include "../../shared/config.hpp"
#include "speak_detection.hpp"
#include <chrono>

namespace speak_detection {
std::string config() { return "speak_detection"; }

namespace {
// frame_size of the model
double frame_size;

// frame_size of the longer speech detection frame
double long_frame_size;

// Sample rate of the recording
double sample_rate;

// How big the window is (in seconds) for the online calculations
double long_frame_seconds;

// The mean energy over a longer period
long long long_energy;

// The mean energy over the prediction frame
long long energy;

// How far above the energy has to be for the inference to be called
double deviation_energy;

// ms to wait before predicting after first speaker identification
int time_delay;

// counter used to determine when to start subtracting from the long frame
long long long_frame_size_seen = 0;
} // namespace

void initialize(int16_t *buffer, size_t buffer_size, size_t _sample_rate) {
  frame_size = (double)buffer_size;
  sample_rate = (double)_sample_rate;

  long_frame_size = sample_rate * long_frame_seconds;

  long_energy = 0;
  energy = 0;
}

void slide(int16_t dropped_sample, int16_t added_sample) {

  if (long_frame_size_seen == long_frame_size) 
    long_energy -= abs(dropped_sample);
  else
    long_frame_size_seen += 1;


  long_energy += abs(added_sample);

  energy -= abs(dropped_sample);
  energy += abs(added_sample);
}

void setup(nlohmann::json config) {
  long_frame_seconds = config::get_optional_config<double>(
      config, "long_frame_seconds", /*tag=*/"speak_detection",
      /*default=*/30.0);

  deviation_energy = config::get_optional_config<double>(
      config, "deviation_energy", /*tag=*/"speak_detection", /*default=*/5);

  time_delay = config::get_optional_config<int>(
      config, "time_delay ms", /*tag=*/"speak_detection", /*default=*/400);
}

auto timestamp = std::chrono::steady_clock::now();

bool has_speaker() {
  double frame_average_energy = (double)energy / frame_size;
  double long_frame_average_energy = (double)long_energy / long_frame_size_seen;

  LOG(INFO) << TAG("speak_detection") << AixLog::Color::YELLOW << "energy "
            << frame_average_energy << " long energy: " << long_frame_average_energy
            << AixLog::Color::NONE << std::endl;

  auto current_time = std::chrono::steady_clock::now();

  bool speaker_is_detected = false;

  // If the frame_average_energy is double that of the long_frame_average_energy
  // it is worth to try to predict, this will work bad for noisy background though
  speaker_is_detected = speaker_is_detected | (frame_average_energy > long_frame_average_energy * 2);

  if (speaker_is_detected) {
    // If enough time has passed, e.g don't want to predict on first syllable on
    // utterance
    if (std::chrono::duration_cast<std::chrono::milliseconds>(current_time -
                                                              timestamp)
            .count() > time_delay)
      return true;

  } else {
    timestamp = current_time;
  }

  return false;
}

void clear() {}

} // namespace speak_detection
