#include "../../../shared/json.hpp"
#include <string>
#include <vector>

namespace goldfish {

struct model_prediction {
  std::vector<float> probabilities;
};

std::string config();

void setup(const nlohmann::json config);
model_prediction predict(int16_t *audio, size_t size);
void cleanup();

// Dimension of output tensor
size_t prediction_dim();

// Dimension of input tensor
size_t input_dim();

}; // namespace goldfish
