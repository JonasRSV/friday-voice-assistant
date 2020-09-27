#ifndef REPLAY_BUFFER_HPP_SPHZUABX
#define REPLAY_BUFFER_HPP_SPHZUABX

#include "../../shared/json.hpp"
#include <string>

namespace replay_buffer {
  void setup(nlohmann::json config);
  void purge();
  void add(int16_t *audio, size_t size);
  void clear();
  int16_t* next_sample();
  size_t frame_size();
  std::string config();
}
#endif /* end of include guard: REPLAY_BUFFER_HPP_SPHZUABX */
