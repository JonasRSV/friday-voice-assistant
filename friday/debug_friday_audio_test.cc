#include "audio/keyword_detection/goldfish/goldfish.hpp"
#include "audio/recording/recording.hpp"
#include "audio/replay_buffer/replay_buffer.hpp"
#include "audio/replay_buffer/speak_detection.hpp"
#include "iostream"
#include "setup/friday_options.hpp"
#include "shared/config.hpp"
#include "testing/catch.hpp"

template <typename T>
void write_binary(T data, int bytes, std::ofstream &stream) {
  const char *out = reinterpret_cast<const char *>(&data);
  stream.write(out, bytes);
}

TEST_CASE("Run Main Friday simulation and store file", "friday") {
  const char *argv[3];
  argv[1] = "--configs=configs";
  argv[2] = "--logging=5";
  launch::options *opt = launch::parse_options(3, argv);

  // Setup all modules
  goldfish::setup(config::get_launch_config(opt, goldfish::config(),
                                            "debug_friday_audio_test"));
  recording::setup(config::get_launch_config(opt, recording::config(),
                                             "debug_friday_audio_test"));

  // speaker then replay
  speak_detection::setup(config::get_launch_config(
      opt, speak_detection::config(), "debug_friday_audio_test"));
  replay_buffer::setup(config::get_launch_config(opt, replay_buffer::config(),
                                                 "debug_friday_audio_test"));

  LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
            << "Setup Success" << AixLog::Color::NONE << std::endl;

  size_t sample_rate = recording::sample_rate();
  // 20 seconds of audio
  size_t audio_buffer_size = sample_rate * 25;

  int16_t audio_buffer[audio_buffer_size];

  size_t recorded_audio = 0;
  size_t recording_frame_size = replay_buffer::frame_size();

  LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
            << "Recording " << audio_buffer_size << " With sample rate "
            << sample_rate << AixLog::Color::NONE << std::endl;

  // Record audio, as if we were the keyword_detection
  while (recorded_audio < audio_buffer_size) {
    int16_t *sample = replay_buffer::next_sample();

    goldfish::predict(sample, 16000);

    LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
              << "Got a new sample" << AixLog::Color::NONE << std::endl;

    size_t copy_to = 0;
    if (audio_buffer_size - recorded_audio > recording_frame_size)
      copy_to = recorded_audio + recording_frame_size;
    else
      copy_to = audio_buffer_size;

    for (size_t i = 0; recorded_audio < copy_to; recorded_audio++)
      audio_buffer[recorded_audio] = sample[++i];

    recorded_audio = copy_to;

    LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
              << "Recorded " << recorded_audio << " / " << audio_buffer_size
              << AixLog::Color::NONE << std::endl;
  }

  LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
            << "Done Recording" << AixLog::Color::NONE << std::endl;

  // Now we save the recorded audio to a wav-file

  std::ofstream os("/tmp/friday-debug.wav", std::ios::out | std::ios::binary);

  if (!os) {
    LOG(FATAL) << TAG("debug_friday_audio_test") << AixLog::Color::RED
               << "Failed to open file"
               << "/tmp/friday-debug.wav" << AixLog::Color::NONE << std::endl;
  }

  // Write wav header
  // 4 byte file-format
  os.write("RIFF", 4);
  // 4 byte size of file, + 36 to include header
  write_binary((int32_t)(audio_buffer_size + 36), 4, os);
  // 4 more byte of format
  os.write("WAVE", 4);
  // 4 byte of format
  os.write("fmt ", 4);
  // length of format data above
  write_binary((int32_t)16, 4, os);
  // format type, 1 is pcm
  write_binary((int16_t)1, 2, os);
  // Number of channels
  write_binary((int16_t)1, 2, os);
  // Sample rate
  write_binary((int32_t)sample_rate, 4, os);
  // Sample rate * bits per sample * channels / 8
  write_binary((int32_t)(sample_rate * 2), 4, os);
  // Bits per sample * Channels / 8
  write_binary((int16_t)2, 2, os);
  // Bits per sample
  write_binary((int16_t)16, 2, os);
  // Start of data section
  os.write("data", 4);
  // file size
  write_binary((int32_t)(audio_buffer_size), 4, os);

  LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
            << "Write header" << AixLog::Color::NONE << std::endl;

  for (size_t i = 0; i < audio_buffer_size; i++)
    write_binary((int16_t)audio_buffer[i], 2, os);

  os.flush();
  os.close();

  LOG(INFO) << TAG("debug_friday_audio_test") << AixLog::Color::GREEN
            << "Saved wav file to"
            << "/tmp/friday-debug.wav" << AixLog::Color::NONE << std::endl;
}
