#include "../../shared/aixlog.hpp"
#include "../../shared/config.hpp"
#include "../../shared/json.hpp"
#include "recording.hpp"
#include <alsa/asoundlib.h>
#include <stdio.h>

namespace recording {

std::string config() { return "recording"; }

// alsa state that is constructed by setup audio device
snd_pcm_t *alsa_handle;
int16_t *pcm;
int32_t alsa_frame_length = 0;
size_t alsa_sample_rate;

namespace {
void log_error_exit(std::string tag, std::string description,
                    std::string message) {
  LOG(FATAL) << TAG(tag) << description << " " << AixLog::Color::RED << message
             << AixLog::Color::NONE << std::endl;
  exit(1);
}

} // namespace

void setup(nlohmann::json config) {

  alsa_frame_length = config::get_required_config<int>(config, "frame_length",
                                                       /*tag=*/"recording");

  std::string input_audio_device = config::get_required_config<std::string>(
      config, "device", /*tag=*/"recording");
  alsa_sample_rate = config::get_required_config<int>(config, "sample_rate",
                                                      /*tag=*/"recording");

  int error_code = snd_pcm_open(&alsa_handle, input_audio_device.c_str(),
                                SND_PCM_STREAM_CAPTURE, 0);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_open' failed with",
                   snd_strerror(error_code));
  }

  snd_pcm_hw_params_t *hardware_params;
  error_code = snd_pcm_hw_params_malloc(&hardware_params);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_hw_params_malloc' failed with",
                   snd_strerror(error_code));
  }

  error_code = snd_pcm_hw_params_any(alsa_handle, hardware_params);
  // if (error_code != 0) {
  //  log_error_exit("audio setup", "'snd_pcm_hw_params_any' failed with",
  //                 snd_strerror(error_code));
  //}

  error_code = snd_pcm_hw_params_set_access(alsa_handle, hardware_params,
                                            SND_PCM_ACCESS_RW_INTERLEAVED);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_hw_params_set_access' failed with",
                   snd_strerror(error_code));
  }

  error_code = snd_pcm_hw_params_set_format(alsa_handle, hardware_params,
                                            SND_PCM_FORMAT_S16_LE);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_hw_params_set_format' failed with",
                   snd_strerror(error_code));
  }

  error_code = snd_pcm_hw_params_set_rate(alsa_handle, hardware_params,
                                          alsa_sample_rate, 0);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_hw_params_set_rate' failed with",
                   snd_strerror(error_code));
  }

  error_code = snd_pcm_hw_params_set_channels(alsa_handle, hardware_params, 1);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_hw_params_set_channels' failed with",
                   snd_strerror(error_code));
  }

  error_code = snd_pcm_hw_params(alsa_handle, hardware_params);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_hw_params' failed with",
                   snd_strerror(error_code));
  }

  snd_pcm_hw_params_free(hardware_params);

  error_code = snd_pcm_prepare(alsa_handle);
  if (error_code != 0) {
    log_error_exit("recording", "'snd_pcm_prepare' failed with",
                   snd_strerror(error_code));
  }

  pcm = (int16_t *)calloc(alsa_frame_length, sizeof(int16_t));

  if (!pcm) {
    log_error_exit("recording", "failed to allocate memory for audio buffer",
                   "");
  }

  LOG(DEBUG) << TAG("recording") << AixLog::Color::GREEN << "Setup Success"
             << AixLog::Color::NONE << std::endl;
}

void free_recording_device() {
  free(pcm);
  snd_pcm_close(alsa_handle);
}

int16_t *get_next_audio_frame(void) {
  int count = snd_pcm_readi(alsa_handle, pcm, alsa_frame_length);
  if (count < 0) {
    log_error_exit("read audio", "'snd_pcm_readi' failed with",
                   snd_strerror(count));
  } else if (count != alsa_frame_length) {
    LOG(FATAL) << TAG("getting audio frame") << AixLog::Color::RED << "read "
               << count << " frames instead of " << alsa_frame_length
               << AixLog::Color::NONE << std::endl;
    exit(1);
  }
  return pcm;
}

size_t frame_size() { return alsa_frame_length; }
size_t sample_rate() { return alsa_sample_rate; }
} // namespace recording
