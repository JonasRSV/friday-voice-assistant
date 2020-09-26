#include "audio/keyword_detection/goldfish/goldfish.hpp"
#include "audio/recording/recording.hpp"
#include "setup/friday_options.hpp"
#include "shared/aixlog.hpp"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile bool run = true;

void interrupt_handler(int _) {
  (void)_;
  run = false;
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

  size_t frame_size = recording::frame_size();
  std::cout << "Frame Size " << frame_size << std::endl;
  while (run) {
    int16_t *pcm = recording::get_next_audio_frame();

    goldfish::model_prediction pred = goldfish::predict(pcm, frame_size);

    LOG(INFO) << TAG("main") << AixLog::Color::GREEN
              << "Predicted: " << pred.index << AixLog::Color::NONE
              << std::endl;
  }

  recording::free_recording_device();
  launch::free_options(opt);

  return 0;
}
