#include "../shared/aixlog.hpp"

namespace launch {
void set_logging(int logging_level) {
  std::string log_level_description;
  auto logging_severity = AixLog::Severity::info;
  if (logging_level == 0) {
    log_level_description = "fatal";
    logging_severity = AixLog::Severity::fatal;
  } else if (logging_level == 1) {
    log_level_description = "error";
    logging_severity = AixLog::Severity::error;
  } else if (logging_level == 2) {
    log_level_description = "warning";
    logging_severity = AixLog::Severity::warning;
  } else if (logging_level == 3) {
    log_level_description = "info";
    logging_severity = AixLog::Severity::info;
  } else {
    log_level_description = "debug";
    logging_severity = AixLog::Severity::debug;
  }

  AixLog::Log::init<AixLog::SinkCout>(logging_severity,
                                      "%Y-%m-%d %H-%M-%S.#ms [#severity] (#tag) #message");

  LOG(INFO) << TAG("launch") << AixLog::Color::YELLOW << "Log-Level set to "
            << log_level_description << AixLog::Color::NONE << std::endl;
}
} // namespace launch
