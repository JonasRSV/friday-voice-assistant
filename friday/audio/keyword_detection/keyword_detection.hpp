#include "../../shared/json.hpp"
#include <optional>
#include <string>

namespace keyword_detection {
std::string config();
void setup(nlohmann::json config);

// blocks until a prediction is done
std::string prediction();
void clear();
} // namespace keyword_detection
