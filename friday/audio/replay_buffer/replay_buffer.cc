#include "../../shared/aixlog.hpp"
#include "replay_buffer.hpp"
#include "time.h"

namespace replay_buffer {
std::string config() { return "replay_buffer"; }

namespace {

// Buffer we store all audio in
size_t buffer_size;
int16_t *buffer;
int16_t *return_buffer;

// Frame seeker pointers
size_t fr_bk_pt;
size_t fr_fw_pt;

// Audio pointers
size_t au_bk_pt;
size_t au_fw_pt;

size_t audio_frame_size;
size_t max_predict_delay;
size_t predict_jump;

double energy_barrier;

template <typename T>
T get_required_config(nlohmann::json config, std::string name) {
  if (config.contains(name)) {
    return (T)config[name];
  }

  LOG(FATAL) << TAG("replay_buffer") << AixLog::Color::RED << "Required field"
             << name << "not in config" << AixLog::Color::NONE << std::endl;
  exit(1);
}

template <typename T>
T get_optional_config(nlohmann::json config, std::string name, T default_) {
  if (config.contains(name)) {
    return (T)config[name];
  }

  LOG(DEBUG) << TAG("replay_buffer") << AixLog::Color::RED << "Optional field "
             << name << " not in config, using default: " << default_
             << AixLog::Color::NONE << std::endl;

  return default_;
}

void move_ptr_distance(size_t *ptr, size_t distance) {
  *ptr = (*ptr + distance) % buffer_size;
}

size_t distance_between_ptr(size_t *a, size_t *b) {
  // If b is smaller than a it has wrapped around
  if (*b < *a) {
    return buffer_size - *a + *b;
  }

  return *b - *a;
}

void copy_audio_to_buffer(size_t *a, size_t *b, int16_t *audio_buffer) {
  // This assumes that distance between a & b is the same as size of
  // audio_buffer

  // If it has wrapped around we copy from a to the end then from begining to b
  if (*b < *a) {
    size_t a_to_end = buffer_size - *a;

    // copy from a to the end
    memcpy(buffer + *a, audio_buffer, a_to_end * sizeof(int16_t));

    // copy from begining to b
    memcpy(buffer, audio_buffer + a_to_end, (*b) * sizeof(int16_t));
  } else {
    // Otherwise just copy between a and b
    memcpy(buffer + *a, audio_buffer, (*b - *a) * sizeof(int16_t));
  }
}

void copy_buffer_to_audio(size_t *a, size_t *b, int16_t *audio_buffer) {
  // This assumes that distance between a & b is the same as size of
  // audio_buffer

  // If it has wrapped around we copy from a to the end then from begining to b
  if (*b < *a) {
    size_t a_to_end = buffer_size - *a;

    // copy from a to the end
    memcpy(audio_buffer, buffer + *a, a_to_end * sizeof(int16_t));

    // copy from begining to b
    memcpy(audio_buffer + a_to_end, buffer, (*b) * sizeof(int16_t));
  } else {
    // Otherwise just copy between a and b
    memcpy(audio_buffer, buffer + *a, (*b - *a) * sizeof(int16_t));
  }
}

void ensure_frame_pointers_are_in_ok_position() {
  // This ensures that the frame_pointers is in a valid position
  // This currently means that they are not too far away form the audio pointers

  // Ensure that the frame back pointer is at most "max_predict_delay" distance
  // from audio forward pointer
  size_t d = distance_between_ptr(&fr_fw_pt, &au_bk_pt);

  // If distance is to large, move frame to within range.
  if (d > max_predict_delay) {
    size_t distance_to_move = d - max_predict_delay;

    move_ptr_distance(&fr_bk_pt, distance_to_move);
    move_ptr_distance(&fr_fw_pt, distance_to_move);
  }

  d = distance_between_ptr(&fr_bk_pt, &au_bk_pt);
}
double find_next_frame() {
  // Get the frame that has the highest energy

  // First calculate energy of current frame
  long long energy = 0;
  size_t ptr = fr_bk_pt;
  while (distance_between_ptr(&ptr, &fr_fw_pt) > 0) {

    energy += ((long long)buffer[ptr] * (long long)buffer[ptr]);
    move_ptr_distance(&ptr, 1);
  }

  // Remember the best ptrs and energy
  size_t best_bk_ptr = fr_bk_pt, best_fw_ptr = fr_fw_pt;
  long long best_energy = energy;
  // While fr is behind audio, we seek
  while (distance_between_ptr(&fr_fw_pt, &au_bk_pt) > 1) {
    // Increment ptrs
    move_ptr_distance(&fr_bk_pt, 1);
    move_ptr_distance(&fr_fw_pt, 1);

    // Add the energy from the new signal
    energy += ((long long)buffer[fr_fw_pt] * (long long)buffer[fr_fw_pt]);

    // Remove the energy from old signal
    energy -= ((long long)buffer[fr_bk_pt] * (long long)buffer[fr_bk_pt]);

    // if energy is larger, return it
    if (energy > best_energy) {
      best_energy = energy;
      best_bk_ptr = fr_bk_pt;
      best_fw_ptr = fr_fw_pt;
    }
  }

  // Set fw ptrs to the best ptrs and return energy
  fr_bk_pt = best_bk_ptr;
  fr_fw_pt = best_fw_ptr;

  return sqrt((double)best_energy / (double)audio_frame_size);
}
} // namespace

void setup(nlohmann::json config) {
  buffer_size = get_required_config<size_t>(config, "buffer_size");
  audio_frame_size = get_required_config<size_t>(config, "frame_size");

  // The maximum distance allowed between the fr_bk_pt and au_fw_pt
  max_predict_delay = get_required_config<size_t>(config, "max_predict_delay");
  predict_jump = get_optional_config<size_t>(config, "predict_jump", 500);
  energy_barrier = get_optional_config<double>(config, "energy_barrier", 500.0);

  fr_bk_pt = 0;
  fr_fw_pt = audio_frame_size;

  au_bk_pt = 0;
  au_fw_pt = audio_frame_size;

  buffer = (int16_t *)calloc(buffer_size, sizeof(int16_t));
  return_buffer = (int16_t *)calloc(audio_frame_size, sizeof(int16_t));
}
void purge() {
  fr_bk_pt = 0;
  fr_fw_pt = audio_frame_size;

  au_bk_pt = 0;
  au_fw_pt = audio_frame_size;
}

void add(int16_t *audio, size_t size) {
  LOG(DEBUG) << TAG("replay_buffer") << AixLog::Color::YELLOW
             << "adding between  " << au_bk_pt << " to  " << au_fw_pt
             << AixLog::Color::NONE << std::endl;

  copy_audio_to_buffer(&au_bk_pt, &au_fw_pt, audio);

  move_ptr_distance(&au_bk_pt, size);
  move_ptr_distance(&au_fw_pt, size);

}

void clear() {
  if (buffer) {
    delete[] buffer;
  }
}

int16_t *next_sample() {
  // Seek for interesting frames while distance between fr_fw_pt and au_bk_pt is
  // lg than 1

  // This makes sure the fr_ptrs are in a valid position
  ensure_frame_pointers_are_in_ok_position();

  // This updates the fr_ptrs to the frame with highest energy, and returns that
  // frame.
  double energy = find_next_frame();

  LOG(DEBUG) << TAG("replay_buffer") << AixLog::Color::YELLOW
             << "next sample between - " << fr_bk_pt << " to " << fr_fw_pt
             << " energy " << energy << AixLog::Color::NONE << std::endl;

  if (energy > energy_barrier) {
    // We have enough energy

    copy_buffer_to_audio(&fr_bk_pt, &fr_fw_pt, return_buffer);

    move_ptr_distance(&fr_bk_pt, predict_jump);
    move_ptr_distance(&fr_fw_pt, predict_jump);

    return return_buffer;
  }

  return nullptr;
}
size_t frame_size() { return 0; }

} // namespace replay_buffer
