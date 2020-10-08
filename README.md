# Friday voice assistant 


Friday is a always listening, offline, keyword spotting assistant. 


Why would you want to use it?
- Low latency, because it is all offline
- Privacy, your voice is sent nowhere only stored for a few seconds in a buffer in RAM
- No wake-word 

A use case, what I use it for, is to turn on and off my lights at home by saying

```bash
släck ljuset | tänd ljuset
```

Which is Swedish for turn of the light and turn on the lights.

I intend to upload pre-compiled versions with pre-trained models for different platforms, if there exist any interest for it. 

I also intend to try to make it easy to add custom keyword to the underlying keyword spotter. Today, to add custom keywords,  you have to record about 100 samples yourself, it takes less than an hour. But then you have to retrain and all of that and it gets pretty tedious. I hope to make that easier in the future. But if you do want to add custom keywords now, please use this: [Record your own](https://github.com/JonasRSV/friday-voice-assistant/blob/master/friday/audio/keyword_detection/goldfish/docs/preprocessing.md#recording-your-own), it is what I used.


### TOC

- [Dependencies](#dependencies)
- [Features](#features)
- [Audio Device](#audio-device)
- [Launching Friday](#launching-friday)
---

### Dependencies

This project depends on libasound2, there is an [issue](https://github.com/JonasRSV/friday-voice-assistant/issues/1) for adding it into the project, but until then one has to install it.

```bash
sudo apt-get install libasound2-dev
```

The libasound2 lib is usually installed on most Linux systems already, but if it isn't for some reason, run:

```bash
sudo apt-get install libasound2
```


This project also depends on tensorflowlib, there is an [issue](https://github.com/JonasRSV/friday-voice-assistant/issues/2) for adding it into the project, but until then one has to add it manually, See [goldfish setup](https://github.com/JonasRSV/friday-voice-assistant/blob/master/friday/audio/keyword_detection/goldfish/README.md) for instructions on adding it for different platforms.

---

### Features

- [philips-hue](https://github.com/JonasRSV/friday-voice-assistant/blob/master/friday/third_party/philips-hue/README.md)

Currently you can only use this to control philips hue lights. I personally use it installed on a raspberry-pi to control my lights at home. 

I intend to add many more features in the future. You can take a look in [friday/third_party](friday/third_party) how philips hue is implemented if you want to contribute something yourself! :) 

---


### Audio Device

By default Friday will pick the "default" Alsa device, this is specified in [configs/recording.json](configs/recording.json). If you want friday to use some different
recording device: update the field 'device' in [configs/recording.json](configs/recording.json)

You can get a list of available devices with

```bash
arecord -L
```

On some systems you might have to run that with sudo, and then also run Friday with sudo if you as a user cannot see the audio devices.


### Launching Friday


Compile the binary, this was developed with bazel 3.4.1 but have also worked with other versions. Please make sure you have all the dependencies setup before trying to compile.

compiling on linux laptop

```bash
bazel build //friday:friday --define platform=linux-x86-cpu
```

Compiling on raspberry pi

```bash
bazel build //friday:friday --define platform=linux-arm-cpu
```

Then launch with 


```bash
# Set log-level to 5 for debug logging if something is not working

./bazel-bin/friday/friday
  --configs=configs\
  --logging=3\
```




