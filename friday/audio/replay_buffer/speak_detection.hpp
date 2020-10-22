
#ifndef SPEAK_DETECTION_HPP_H731MEPA
#define SPEAK_DETECTION_HPP_H731MEPA

#include <cstdint>
#include "../../shared/json.hpp"
#include "../../shared/aixlog.hpp"

namespace speak_detection {

std::string config();
void setup(nlohmann::json);

// Initialize state
void initialize(int16_t *buffer, size_t buffer_size, size_t sample_rate);

// Update the state: Added sample is now in the frame and dropped is out.
// This function is called once per audio sample so it has to be very fast.
void slide(int16_t dropped_sample, int16_t added_sample);

// Returns true if a speaker is detected in the current frame
bool has_speaker();

// Clear any resources used by speak_detection
void clear();
}

#endif /* end of include guard: SPEAK_DETECTION_HPP_H731MEPA */
