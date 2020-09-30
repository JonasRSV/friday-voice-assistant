# THIS IS A WORK IN PROGRESS 

The following content is incomplete and outdated

---

# Friday voice assistant 

simple keyword assistant (WIP)

- [Requirements](#requirements)
- [Features](#features)
  - [Philips Hue](#philips-hue) 
- [Launching Friday](#launching-friday)
---

### Requirements

```bash
sudo apt-get install libasound2-dev
```

Also please download lib-tensorflow, it is too big for me to be allowed to push it to github

```bash
curl  https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-1.14.0.tar.gz > libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz
```

open it up with

```bash
tar -xvf libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz
```

and copy the lib files into 

```bash
friday/audio/keyword_detection/goldfish/cppflow/tensorflow/lib
```


---

### Features

#### Philips Hue

Begin by finding the ip of the hue bridge, with nast installed you can run

```bash
HUE_IP=$(sudo nast -m | grep hue | tail -1 | grep -E -o '(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)')
```

A user needs to be created for friday. Press the button on the hue hub then run

```bash
curl -X POST -d '{"devicetype": "friday"}' ${HUE_IP?}/api
```

To list all available lights and their information run 

```bash
curl ${HUE_IP?}/api/USER_HERE/lights
```

---

### Launching Friday

Begin by compiling the binary

```bash
bazel build //friday:friday
```

Then launch with 


```bash
# Set log-level to 5 for debug logging if something is not working
# List audio-device with 'arecord -L'

./bazel-bin/friday/friday
  --configs=configs\
  --logging=3\
```




