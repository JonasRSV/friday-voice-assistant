# Goldfish

- [Developing](#developing)
    - [Dependencies](#dependencies)
- [Setup](#setup)
- [Testing](#testing)

Goldfish is the keyword spotting model used by friday. For instructions on training your own see [preprocessing](preprocessing.md) 
and [model_golding](model_golding.md). 

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

### Setup

Setting up goldfish requires some manual downloading. 

The problem is that goldfish uses libtensorflow as precompiled library files. These files are close to 1GB in size and github's maximum allowed file-size is 100MB, so I cannot push them.  **What you need** is libtensorflow 1.14 library files in an appropriate place (which is described below).

Unless this README is very old you should be able to get files from the google api, the following is an example for downloading the linux-x86-64-gpu:

```bash
curl  https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz > libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz
```

Getting the CPU should be as easy as replacing the 'gpu' in the url with 'cpu'. Using CPU is fine, on my machine (Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz) running the assistant uses about 3% of my CPU. 

Open the tar file using:

```bash
tar -xvf libtensorflow-gpu-linux-x86_64-1.14.0.tar.gz
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

we are only interested in the library files since the includes are already available at [friday/audio/keyword_detection/goldfish/tf/tensorflow/include](https://github.com/JonasRSV/friday-voice-assistant/tree/master/friday/audio/keyword_detection/goldfish/tf/tensorflow/include/tensorflow/c)

We want to copy the library files into the correct place. If you look at [friday/audio/keyword_detection/goldfish/cppflow/tensorflow/lib](https://github.com/JonasRSV/friday-voice-assistant/tree/master/friday/audio/keyword_detection/goldfish/tf/tensorflow/lib) you should see a few empty directories. For example "linux-x86-64-gpu", if you downloaded the linux GPU 64bit library that is where you should put the library files. 


### Testing

The following command will try to make predictions using a model located at friday/audio/keyword_detection/goldfish/models/saved_models/test_model this will only work if the setup was done corretly, so this serves as a good way of testing your setup.

```bash
bazel test friday/audio/keyword_detection/goldfish:goldfish_test --test_output=all
```

If this test passes, the manual setup went well!
