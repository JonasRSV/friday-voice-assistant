# Friday voice assistant 

A Simple keyword assistant, it is still very WIP.

- [Requirements](#requirements)
- [Features](#features)
- [Launching Friday](#launching-friday)
---

### Dependencies
---

This project depends on libasound2, there is an [issue](https://github.com/JonasRSV/friday-voice-assistant/issues/1) for adding it into the project, but until then one has to install it.

```bash
sudo apt-get install libasound2-dev
```

This project also depends on tensorflowlib, there is an [issue](https://github.com/JonasRSV/friday-voice-assistant/issues/2) for adding it into the project, but until then one has to add it manually, See [goldfish setup](https://github.com/JonasRSV/friday-voice-assistant/blob/master/friday/audio/keyword_detection/goldfish/README.md) for instructions on downloading tensorflowlib.


---

### Features
- [philips-hue](https://github.com/JonasRSV/friday-voice-assistant/blob/master/friday/third_party/philips-hue/README.md)


### Launching Friday

Begin by compiling the binary, this project was developed with bazel 3.4.1, bazel is pretty 
petty about versions so make sure you have the same one.


for CPU

```bash
bazel build //friday:friday --define tf=cpu
```

for GPU

```bash
bazel build //friday:friday --define tf=cpu
```

Then launch with 


```bash
# Set log-level to 5 for debug logging if something is not working

./bazel-bin/friday/friday
  --configs=configs\
  --logging=3\
```




