#include "../../shared/config.hpp"
#include "speak_detection.hpp"
#include <chrono>
#include <queue>

namespace speak_detection {
std::string config() { return "speak_detection"; }

namespace {
// frame_size of the model
double frame_size;

// frame_size of the longer speech detection frame
double long_frame_size;

// Sample rate of the recording
double sample_rate;

// How big the window is (in seconds) of the long frame
double long_frame_seconds;

// The mean energy over a longer period
long long long_energy;

// The mean energy over the prediction frame
long long energy;

// ms to wait before predicting after first speaker identification
int time_delay;

} // namespace

void initialize(int16_t *buffer, size_t buffer_size, size_t _sample_rate) {
  frame_size = (double)buffer_size;
  sample_rate = (double)_sample_rate;

  long_frame_size = sample_rate * long_frame_seconds;

  long_energy = 0;
  energy = 0;
}

std::deque<int16_t> long_frame_queue;
// This function is run once per audio sample, so no heavy computations in
// here... e.g for the normal 8000Hz sample rate we run this 8000x per second,
// not too bad but still... consider it!
void slide(int16_t dropped_sample, int16_t added_sample) {

  // Accumulate the long-frame until weve seen the whole of it.
  if (long_frame_queue.size() == long_frame_size) {
    long_energy -= abs(long_frame_queue.front());

    long_frame_queue.pop_front();
  }

  long_frame_queue.push_back(added_sample);

  long_energy += abs(added_sample);

  energy -= abs(dropped_sample);
  energy += abs(added_sample);
}

void setup(nlohmann::json config) {
  long_frame_seconds = config::get_optional_config<double>(
      config, "long_frame_seconds", /*tag=*/"speak_detection",
      /*default=*/30.0);

  time_delay = config::get_optional_config<int>(
      config, "time_delay ms", /*tag=*/"speak_detection", /*default=*/400);
}

auto timestamp = std::chrono::steady_clock::now();

// If the keyword detector detects a keword in the audio
// we can assume a speaker spoke in it and use it to update
// our has_speaker heuristic.
void was_speaker() {
  LOG(DEBUG) << TAG("speak_detection") << AixLog::Color::YELLOW
             << "speaker was detected " << AixLog::Color::NONE << std::endl;

  // If there was a speaker we will drop the previous "frame size" from the
  // long buffer so it does not increase the "background noise"
  for (size_t i = 0; i < frame_size && long_frame_queue.size(); i++) {
    long_energy -= abs(long_frame_queue.back());
    long_frame_queue.pop_back();
  }
}
void was_no_speaker() {
  LOG(DEBUG) << TAG("speak_detection") << AixLog::Color::YELLOW
             << "no speaker was detected " << AixLog::Color::NONE << std::endl;
}

// This function is used by the replay_buffer to detemine wether
// the model frame contains speaker audio, and if so returns it to
// the keyword detector
bool has_speaker() {
  double frame_average_energy = (double)energy / frame_size;
  double long_frame_average_energy =
      (double)long_energy / (double)long_frame_queue.size();

  LOG(INFO) << TAG("speak_detection") << AixLog::Color::YELLOW << "energy "
            << frame_average_energy
            << " long energy: " << long_frame_average_energy
            << AixLog::Color::NONE << std::endl;

  auto current_time = std::chrono::steady_clock::now();

  bool speaker_is_detected = false;

  // If the frame_average_energy is double that of the
  // long_frame_average_energy it is worth to try to predict, this will work
  // bad for noisy background though since the voice wont double the energy.
  speaker_is_detected =
      (frame_average_energy > long_frame_average_energy * 1.2);

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
