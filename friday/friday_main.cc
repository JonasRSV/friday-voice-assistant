#include "audio/keyword_detection/goldfish/goldfish.hpp"
#include "audio/keyword_detection/keyword_detection.hpp"
#include "audio/playback/playback.hpp"
#include "audio/recording/recording.hpp"
#include "audio/replay_buffer/replay_buffer.hpp"
#include "dispatch/dispatch.hpp"
#include "dispatch/local.hpp"
#include "setup/friday_options.hpp"
#include "shared/aixlog.hpp"
#include "third_party/philips-hue/philips_hue.hpp"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define SECONDS(x) (long long)(x * 1000000)

nlohmann::json get_config(launch::options *opt, std::string name) {
  if (opt->configs.find(name) != opt->configs.end()) {
    return opt->configs[name];
  }

  LOG(FATAL) << TAG("main") << AixLog::Color::RED << "Required config: '"
             << name
             << "' not found -- please make sure there is a json file of that "
                "name in the config directory"
             << AixLog::Color::NONE << std::endl;
  exit(1);
}

void global_setup(launch::options *opt) {
  goldfish::setup(get_config(opt, goldfish::config()));
  recording::setup(get_config(opt, recording::config()));
  replay_buffer::setup(get_config(opt, replay_buffer::config()));
  keyword_detection::setup(get_config(opt, keyword_detection::config()));
  playback::setup(get_config(opt, playback::config()));
  philips_hue::setup(get_config(opt, philips_hue::config()));
  local::setup(get_config(opt, local::config()));
  dispatch::setup(get_config(opt, dispatch::config()));
}

void global_cleanup() {
  keyword_detection::clear();
  replay_buffer::clear();
  recording::free_recording_device();
  playback::free_playback_device();
  launch::free_options();
  philips_hue::free_philips_hue();
}

static volatile bool run = true;

void interrupt_handler(int _) {
  (void)_;

  global_cleanup();
  exit(0);
}

int main(int argc, const char *argv[]) {
  // Stop and cleanup on keyboard interrupt
  signal(SIGINT, interrupt_handler);
  launch::options *opt = launch::parse_options(argc, argv);

  // Setup all modules
  global_setup(opt);

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

    // Dispatches it to execute its commands
    dispatch::dispatch(prediction);

    usleep(SECONDS(2.0));
  }

  return 0;
}
