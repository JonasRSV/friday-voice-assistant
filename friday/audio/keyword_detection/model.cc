#include "model.hpp"
#include <string>
#include "goldfish/goldfish.hpp"
#include "../replay_buffer/replay_buffer.hpp"

#define SECONDS(x) (long long)(x * 1000000)

namespace keyword_detection {
void setup(nlohmann::json config) {

}

std::string prediction() {

  size_t frame_size = replay_buffer::frame_size();
  while (true) {
    int16_t *predict_frame = replay_buffer::next_sample();

    if (predict_frame != nullptr) {
      goldfish::model_prediction pred =
      goldfish::predict(predict_frame, frame_size);

      LOG(INFO) << TAG("main") << AixLog::Color::GREEN
                << "Predicted: " << pred.probabilities[0] << AixLog::Color::NONE
                << std::endl;
    } else {
      LOG(DEBUG) << TAG("main") << AixLog::Color::CYAN
                 << "No audio to predict on" << AixLog::Color::NONE
                 << std::endl;
    }

    usleep(SECONDS(1.0));
}

} // namespace keyword_detection
