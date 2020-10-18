#include "../../shared/aixlog.hpp"
#include "../../shared/config.hpp"
#include "../recording/recording.hpp"
#include "replay_buffer.hpp"
#include "speak_detection.hpp"
#include <atomic>
#include <chrono>
#include <thread>

namespace replay_buffer {
std::string config() { return "replay_buffer"; }

namespace {

#define SECONDS(x) (long long)(x * 1000000)

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

size_t model_frame_size;
size_t audio_frame_size;

std::thread *recording_thread;

// If the audio thread has added a new frame
std::atomic_bool has_new_frame(false);
std::atomic_bool run_listen(true);

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

// Function for moving frame_pointers to next audio frame
void update_frame_pointers() {
  // First we calculate how long our update jump is
  size_t d = distance_between_ptr(&fr_fw_pt, &au_bk_pt);

  // Then we slide across our audio to update the speak_detection state
  // and put frame_pointers in correct place
  // We only slide to d - 1 because that is the newest sample.
  for (size_t i = 0; i < d - 1; i++) {
    int16_t dropped_sample = buffer[fr_bk_pt];

    move_ptr_distance(&fr_bk_pt, 1);
    move_ptr_distance(&fr_fw_pt, 1);
  }
}

void listen() {
  // Read a few frames because the first audio seems to be garbled
  size_t frame_size = recording::frame_size();
  size_t sample_rate = recording::sample_rate();
  // LOG(INFO) << TAG("replay_buffer") << AixLog::Color::GREEN
  //<< "Starting audio recording -- frame_size: " << frame_size
  //<< " -- sample_rate: " << sample_rate << AixLog::Color::NONE
  //<< std::endl;

  // We forget some frames because audio in the beginning of recording is
  // sometimes garbled, for some reason.
  int frames_to_forget = 1 + (int)(sample_rate * 2 / frame_size);
  for (int i = 0; i < frames_to_forget; i++)
    recording::get_next_audio_frame();

  while (run_listen) {
    int16_t *pcm = recording::get_next_audio_frame();
    // LOG(DEBUG) << TAG("replay_buffer") << AixLog::Color::YELLOW
    //<< "Read New Frame " << index.fetch_add(1) << AixLog::Color::NONE
    //<< std::endl;
    replay_buffer::add(pcm, frame_size);
  }
}
} // namespace

void add(int16_t *audio, size_t size) {
  // This is super verbose
  // LOG(DEBUG) << TAG("replay_buffer") << AixLog::Color::YELLOW
  //<< "adding between  " << au_bk_pt << " to  " << au_fw_pt
  //<< AixLog::Color::NONE << std::endl;

  copy_audio_to_buffer(&au_bk_pt, &au_fw_pt, audio);

  move_ptr_distance(&au_bk_pt, size);
  move_ptr_distance(&au_fw_pt, size);

  has_new_frame = true;
  // std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void setup(nlohmann::json config) {
  buffer_size = config::get_required_config<size_t>(config, "buffer_size",
                                                    /*tag=*/"replay_buffer");

  // Frame size model will get
  model_frame_size = config::get_required_config<size_t>(
      config, "frame_size", /*tag=*/"replay_buffer");

  // Frame size recorded by alsa
  audio_frame_size = recording::frame_size();

  fr_bk_pt = 0;
  fr_fw_pt = model_frame_size;

  au_bk_pt = 0;
  au_fw_pt = audio_frame_size;

  buffer = (int16_t *)calloc(buffer_size, sizeof(int16_t));
  return_buffer = (int16_t *)calloc(model_frame_size, sizeof(int16_t));

  // Start new thread for recording audio
  recording_thread = new std::thread(listen);
}
void purge() {
  fr_bk_pt = 0;
  fr_fw_pt = model_frame_size;

  au_bk_pt = 0;
  au_fw_pt = audio_frame_size;
}

void clear() {
  if (buffer) {
    delete[] buffer;
  }

  if (return_buffer) {
    delete[] return_buffer;
  }

  run_listen = false;

  recording_thread->join();

  if (recording_thread) {
    delete recording_thread;
  }
}

bool speaker_detection_initialized = false;

int16_t *next_sample() {
  // Block until we find a sample
  while (true) {
    while (!has_new_frame) {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    // LOG(DEBUG) << TAG("replay_buffer") << AixLog::Color::YELLOW
    //<< "Got new Frame " << index.fetch_add(1) << AixLog::Color::NONE
    //<< std::endl;

    // Need to initialize speak_detection with first frame
    if (!speaker_detection_initialized) {
      // Initialize speaker identification with initial audio
      speak_detection::initialize(buffer, model_frame_size);
      speaker_detection_initialized = true;
    }

    has_new_frame = false;

    // Say that current frame is no longer new

    // This moves the frame_pointers to the new audio frame
    update_frame_pointers();

    // We copy the content between our frame_pointers into the return_buffer
    copy_buffer_to_audio(&fr_bk_pt, &fr_fw_pt, return_buffer);

    // We check if the current frame has a speaker in it
    if (speak_detection::has_speaker(return_buffer, model_frame_size)) {
      return return_buffer;
    }
  }
}
size_t frame_size() { return model_frame_size; }

} // namespace replay_buffer
