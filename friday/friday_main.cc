#include "audio/keyword_detection/goldfish/goldfish.hpp"
#include "audio/keyword_detection/keyword_detection.hpp"
#include "audio/playback/playback.hpp"
#include "audio/recording/recording.hpp"
#include "audio/replay_buffer/replay_buffer.hpp"
#include "audio/replay_buffer/speak_detection.hpp"
#include "dispatch/dispatch.hpp"
#include "dispatch/local.hpp"
#include "setup/friday_options.hpp"
#include "shared/aixlog.hpp"
#include "shared/config.hpp"
#include "third_party/philips-hue/philips_hue.hpp"
#include <chrono>
#include <signal.h>
#include <stdio.h>

void global_setup(launch::options *opt) {
  // The order of these setups matter

  goldfish::setup(config::get_launch_config(opt, goldfish::config(), "main"));
  recording::setup(config::get_launch_config(opt, recording::config(), "main"));
  playback::setup(config::get_launch_config(opt, playback::config(), "main"));

  keyword_detection::setup(
      config::get_launch_config(opt, keyword_detection::config(), "main"));
  philips_hue::setup(
      config::get_launch_config(opt, philips_hue::config(), "main"));
  local::setup(config::get_launch_config(opt, local::config(), "main"));
  dispatch::setup(config::get_launch_config(opt, dispatch::config(), "main"));

  // speaker then replay
  speak_detection::setup(
      config::get_launch_config(opt, speak_detection::config(), "main"));
  replay_buffer::setup(
      config::get_launch_config(opt, replay_buffer::config(), "main"));

  LOG(INFO) << TAG("main") << AixLog::Color::GREEN << "Setup Success"
            << AixLog::Color::NONE << std::endl;
}

void global_cleanup() {
  keyword_detection::clear();
  replay_buffer::clear();
  recording::free_recording_device();
  playback::free_playback_device();
  launch::free_options();
  philips_hue::free_philips_hue();

  LOG(INFO) << TAG("main") << AixLog::Color::MAGENTA << "Cleanup done.. Bye!"
            << AixLog::Color::NONE << std::endl;
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

  LOG(INFO) << TAG("main") << AixLog::Color::GREEN << "Starting to listen.. "
            << AixLog::Color::NONE << std::endl;

  while (true) {
    std::string prediction = keyword_detection::prediction();

    LOG(INFO) << TAG("main") << AixLog::Color::GREEN
              << "Predicted: " << prediction << AixLog::Color::NONE
              << std::endl;

    // Dispatches it to execute its commands
    dispatch::dispatch(prediction);

    // Sleep to clear buffer
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  }

  return 0;
}
