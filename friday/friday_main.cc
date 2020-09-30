#include "audio/keyword_detection/goldfish/goldfish.hpp"
#include "audio/keyword_detection/keyword_detection.hpp"
#include "audio/playback/playback.hpp"
#include "audio/recording/recording.hpp"
#include "audio/replay_buffer/replay_buffer.hpp"
#include "setup/friday_options.hpp"
#include "shared/aixlog.hpp"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define SECONDS(x) (long long)(x * 1000000)

void clean() {
  keyword_detection::clear();
  replay_buffer::clear();
  recording::free_recording_device();
  launch::free_options();
}

static volatile bool run = true;

void interrupt_handler(int _) {
  (void)_;

  clean();
  exit(0);
}

nlohmann::json get_config(launch::options *opt, std::string name) {
  if (opt->configs.find(name) != opt->configs.end()) {
    return opt->configs[name];
  }

  LOG(FATAL) << TAG("main") << AixLog::Color::RED << "Required config" << name
             << "not loaded" << AixLog::Color::NONE << std::endl;
  exit(1);
}

int main(int argc, const char *argv[]) {
  // Stop and cleanup on keyboard interrupt
  signal(SIGINT, interrupt_handler);
  launch::options *opt = launch::parse_options(argc, argv);

  goldfish::setup(get_config(opt, goldfish::config()));
  recording::setup(get_config(opt, recording::config()));
  replay_buffer::setup(get_config(opt, replay_buffer::config()));
  keyword_detection::setup(get_config(opt, keyword_detection::config()));
  playback::setup(get_config(opt, playback::config()));

  LOG(INFO) << TAG("main") << AixLog::Color::GREEN << "Purging audio buffer.. "
            << AixLog::Color::NONE << std::endl;
  // to let replay_buffer recording get started
  usleep(SECONDS(2));

  LOG(INFO) << TAG("main") << AixLog::Color::GREEN << "Starting to listen.. "
            << AixLog::Color::NONE << std::endl;

  while (true) {
    std::string prediction = keyword_detection::prediction();

    LOG(INFO) << TAG("main") << AixLog::Color::GREEN
              << "Predicted: " << prediction << AixLog::Color::NONE
              << std::endl;

    usleep(SECONDS(2.0));
  }

  return 0;
}
