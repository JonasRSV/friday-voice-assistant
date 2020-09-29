#ifndef RECORDING_HPP_CBL1HWMM
#define RECORDING_HPP_CBL1HWMM

#include "../../shared/json.hpp"
#include <alsa/asoundlib.h>

namespace recording {
std::string config();
void setup(nlohmann::json config);
void free_recording_device();
int16_t *get_next_audio_frame(void);
size_t frame_size();
size_t sample_rate();
} // namespace recording
#endif /* end of include guard: RECORDING_HPP_CBL1HWMM */
