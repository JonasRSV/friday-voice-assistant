#include <string>

#include "../../shared/aixlog.hpp"
#include "../../shared/config.hpp"
#include "../../shared/json.hpp"
#include "playback.hpp"

namespace playback {
std::string config() { return "playback"; }
snd_pcm_t *pcm_handle;
size_t buffer_size;
int16_t *playback_buffer;

namespace {
void log_error_exit(std::string tag, std::string description,
                    std::string message) {
  LOG(FATAL) << TAG(tag) << description << " " << AixLog::Color::RED << message
             << AixLog::Color::NONE << std::endl;
  exit(1);
}

} // namespace

void setup(nlohmann::json config) {
  std::string device =
      config::get_optional_config<std::string>(config,
                                               /*name=*/"device",
                                               /*tag=*/"playback",
                                               /*default=*/"default");
  unsigned int sample_rate =
      config::get_optional_config<unsigned int>(config,
                                                /*name=*/"sample_rate",
                                                /*tag=*/"playback",
                                                /*default=*/8000);

  buffer_size = config::get_optional_config<size_t>(config,
                                                    /*name=*/"buffer_size",
                                                    /*tag=*/"playback",
                                                    /*default=*/8000);
  int pcm;
  int channels = 1;
  snd_pcm_hw_params_t *params;

  /* Open the PCM device in playback mode */
  if ((pcm = snd_pcm_open(&pcm_handle, device.c_str(), SND_PCM_STREAM_PLAYBACK,
                          0) < 0))
    log_error_exit(/*tag=*/"playback", "ERROR: Can't open PCM device. ",
                   snd_strerror(pcm));

  /* Allocate parameters object and fill it with default values*/
  snd_pcm_hw_params_alloca(&params);

  snd_pcm_hw_params_any(pcm_handle, params);

  /* Set parameters */
  if ((pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
                                          SND_PCM_ACCESS_RW_INTERLEAVED) < 0))
    log_error_exit(/*tag=*/"playback",
                   /*message=*/"ERROR: Can't set interleaved mode.",
                   /*error=*/snd_strerror(pcm));

  if ((pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
                                          SND_PCM_FORMAT_S16_LE) < 0))
    log_error_exit(/*tag=*/"playback", /*message=*/"ERROR: Can't set format.",
                   /*error=*/snd_strerror(pcm));

  if ((pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0))
    log_error_exit(/*tag=*/"playback",
                   /*message=*/"ERROR: Can't set channels number.",
                   /*error=*/snd_strerror(pcm));

  if ((pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate,
                                             0) < 0))
    log_error_exit(/*tag=*/"playback", /*message=*/"ERROR: Can't set rate.",
                   /*error=*/snd_strerror(pcm));

  /* Write parameters */
  if ((pcm = snd_pcm_hw_params(pcm_handle, params) < 0))
    log_error_exit(/*tag=*/"playback",
                   /*message=*/"ERROR: Can't set hardware parameters.",
                   /*error=*/snd_strerror(pcm));

  unsigned int set_channels, set_rate;
  snd_pcm_hw_params_get_channels(params, &set_channels);
  snd_pcm_hw_params_get_rate(params, &set_rate, 0);

  LOG(INFO) << TAG("playback") << AixLog::Color::MAGENTA
            << "PCM name: " << snd_pcm_name(pcm_handle) << " -- PCM state: "
            << snd_pcm_state_name(snd_pcm_state(pcm_handle))
            << " -- Channels: " << set_channels << " -- Rate: " << set_rate
            << AixLog::Color::NONE << std::endl;

  LOG(INFO) << TAG("playback") << AixLog::Color::GREEN << "Setup Success"
            << AixLog::Color::NONE << std::endl;

  playback_buffer = (int16_t *)calloc(buffer_size, sizeof(int16_t));
}

void play_audio_frame(int16_t *audio, size_t size, unsigned int sample_rate) {
  memset(playback_buffer, 0, sizeof(int16_t) * buffer_size);

  size_t pcm_code;

  size_t played_audio = 0;
  snd_pcm_prepare(pcm_handle);
  while (played_audio < size) {
    size_t audio_to_copy = 0;
    if (size - played_audio > buffer_size) {
      audio_to_copy = buffer_size;
    } else {
      audio_to_copy = size - played_audio;
    }

    memcpy(playback_buffer, audio + played_audio,
           audio_to_copy * sizeof(int16_t));

    if ((pcm_code = snd_pcm_writei(pcm_handle, playback_buffer, buffer_size) !=
                    (int)buffer_size)) {
      LOG(WARNING) << TAG("playback")
                   << "ERROR. Can't write to PCM device: " << AixLog::Color::RED
                   << snd_strerror(pcm_code) << AixLog::Color::NONE
                   << std::endl;
    }

    played_audio += audio_to_copy;
  }
}

void free_playback_device() {
  snd_pcm_drain(pcm_handle);
  snd_pcm_close(pcm_handle);
  free(playback_buffer);
}
} // namespace playback
