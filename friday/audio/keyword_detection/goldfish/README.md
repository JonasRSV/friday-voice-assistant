# Goldfish

- [Developing](#developing)
    - [Dependencies](#dependencies)
- [Setup](#setup)
- [Testing](#testing)

Goldfish is the keyword spotting model used by friday. For instructions on training your own see [preprocessing](docs/preprocessing.md) 
and [model_magikarp](docs/model_magikarp.md). 

## Developing 

### Dependencies
Bazel does not work that great with python yet, I recommend starting a virtual env in the goldfish directory

```bash
virtualenv .venv -p python3.6 && . .venv/bin/activate
```

Then installing the requirements

```bash
python3 -m pip3 install -r requirements.txt
```

Hopefully the commands in the other docs should work as expected now.

### Setup for usage with friday

#### x86-linux-CPU

Unzip the tensorflow cpu library an you're good to go

```bash
unzip tf/tensorflow/lib/linux-x86-64-cpu/libtf.zip -d tf/tensorflow/lib/linux-x86-64-cpu
```

#### x86-linux-GPU

Unfortunately the GPU library is to big to push to github, even compressed so you'll have to download it and place it in the lib. Unless this README is very old you can do it via the google api

```bash
curl  https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz > /tmp/libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz
```
Open the tar file using:

```bash
tar -xvf /tmp/libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz --directory=/tmp
```

It should contain something like

```bash
./include/
./include/tensorflow/
./include/tensorflow/c/
./include/tensorflow/c/c_api.h
./include/tensorflow/c/c_api_experimental.h
./include/tensorflow/c/tf_attrtype.h
./lib/
./lib/libtensorflow.so.1.14.0
./lib/libtensorflow_framework.so.1.14.0
./lib/libtensorflow_framework.so
./lib/libtensorflow_framework.so.1
./lib/libtensorflow.so
./lib/libtensorflow.so.1
./include/tensorflow/c/LICENSE
./include/tensorflow/c/eager/
./include/tensorflow/c/eager/c_api.h
```

we are only interested in the library files 

```bash
cp /tmp/lib/*.so tf/tensorflow/lib/linux-x86-64-gpu
```

#### Raspberry Pi

Just need to unzip the library

```bash
unzip tf/tensorflow/lib/raspberryPi32/libtf.zip -d tf/tensorflow/lib/raspberryPi32
```

#### Rock64

Just need to unzip the library

```bash
unzip tf/tensorflow/lib/rock64/libtf.zip -d tf/tensorflow/lib/rock64
```


### Testing

```bash
bazel test //friday/audio/keyword_detection/goldfish:goldfish_test --test_output=all
```

If this test passes, the setup went well!
