#include "../../shared/json.hpp"
#include <optional>
#include <string>

namespace keyword_detection {
void setup(nlohmann::json config);

// blocks until a prediction is done
std::string prediction();
} // namespace keyword_detection
