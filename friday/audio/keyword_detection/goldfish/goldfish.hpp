#include "../../../shared/json.hpp"
#include <string>

namespace goldfish {

struct model_prediction {
  int64_t index;
  std::string predicted_string;
};

std::string config();

void setup(const nlohmann::json config);
model_prediction predict(int16_t *audio, size_t size);
void cleanup();

}; // namespace goldfish
