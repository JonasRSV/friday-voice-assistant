#ifndef PLAYBACK_HPP_P6JQUVDC
#define PLAYBACK_HPP_P6JQUVDC

#include "../../shared/json.hpp"
#include <alsa/asoundlib.h>

namespace playback {
std::string config();
void setup(nlohmann::json config);
void free_playback_device();
void play_audio_frame(int16_t *audio, size_t size, unsigned int sample_rate);
} // namespace playback

#endif /* end of include guard: PLAYBACK_HPP_P6JQUVDC   */
