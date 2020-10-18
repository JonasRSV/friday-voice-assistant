## Goldfish Preprocessing

- [Goldfish Voice](#goldfish-voice)
- [Common Voice](#common-voice)
- [Google Speech Commands](#google-speech_commands)
- [Recording your own](#recording-your-own)
- [Cleaning Data](#cleaning-goldfish-voice-data)
- [Splitting](#splitting)

### Goldfish Voice

---

Since we are using tensorflow the data-sources should be converted into a format that is amenable to usage within the tensorflow library. A okay format is tfexamples. 

It is the job of a preprocessing pipeline to convert its source into a tfexample with the following fields

```python3
# The audio fields are x seconds recordings of mono 16 bit PCM data 

audio: int64list # Would be nice with int16 list but tfexample does not support it.. which unfortunately makes the dataset 4x larger.
text:  byteslist
sample_rate: int64 # This should pretty much always be 8kHz or 16kHz
label: int64 # This is optional and not always present.
```

This can then be used to train our keyword detection model.

### Common Voice

Preprocessing depends on SoX

```bash
sudo apt-get install sox && sudo apt-get libsox-fmt-mp3
```


[common voice](https://commonvoice.mozilla.org/sv-SE/datasets) is a open-source dataset provided by mozilla. It contains short to medium size utterances with accompanied text.

#### Common voice sentences

Start by downloading setting the DATASET_PATH and extracting the dataset into it.

```bash
DATASET_PATH=$HOME/.data
mkdir -p $DATASET_PATH
cd $DATASET_PATH \
    && curl https://voice-prod-bundler-ee1969a6ce8178826482b88e843c335139bd3fb4.s3.amazonaws.com/cv-corpus-5.1-2020-06-22/sv-SE.tar.gz -L > sv-SE.tar.gz \
    && tar -xvf sv-SE.tar.gz
```

Then create experiment directory 

```bash
EXPERIMENT_NAME=$(date | tr " " "_")
GOLDFISH_DIRECTORY=${DATASET_PATH?}/friday/goldfish/${EXPERIMENT_NAME?}
mkdir -p $GOLDFISH_DIRECTORY
```

The following script then converts it into Goldfish voice format

```bash
bazel run //friday/audio/keyword_detection/goldfish/preprocessing:common_voice_to_goldfish_voice --\
  --tsv=$DATASET_PATH/cv-corpus-5.1-2020-06-22/sv-SE/validated.tsv \
  --clips=$DATASET_PATH/cv-corpus-5.1-2020-06-22/sv-SE/clips \
  --output_prefix=$GOLDFISH_DIRECTORY/tfexamples.cv_long\
  --sample_rate=8000
```

#### Common Voice Single Word

Start by downloading setting the DATASET_PATH and extracting the dataset into it.

```bash 
DATASET_PATH=$HOME/.data
mkdir -p $DATASET_PATH
cd $DATASET_PATH \
    && curl https://cdn.commonvoice.mozilla.org/cv-corpus-5.1-singleword/cv-corpus-5.1-singleword.tar.gz > cv_single_word.tar.gz\
    && tar -xvf cv_single_word.tar.gz
```

Then create experiment directory 

```bash
EXPERIMENT_NAME=$(date | tr " " "_")
GOLDFISH_DIRECTORY=${DATASET_PATH?}/friday/goldfish/${EXPERIMENT_NAME?}
mkdir -p $GOLDFISH_DIRECTORY
```

The following script then converts it into Goldfish voice format, one needs to set $LANGUAGE


```bash
bazel run //friday/audio/keyword_detection/goldfish/preprocessing:common_voice_to_goldfish_voice --\
  --tsv=$DATASET_PATH/cv-corpus-5.1-singleword/${LANGUAGE?}/validated.tsv \
  --clips=$DATASET_PATH/cv-corpus-5.1-singleword/${LANGUAGE?}/clips \
  --output_prefix=$GOLDFISH_DIRECTORY/tfexamples.cv_short
```

### Google Speech Commands

```bash
DATASET_PATH=$HOME/.data
mkdir -p $DATASET_PATH/google_speech_commands
cd $DATASET_PATH/google_speech_commands \
    && curl -L http://download.tensorflow.org/data/speech_commands_v0.02.tar.gz > speech_commands.tar.gz \
    && tar -xvf speech_commands.tar.gz
```

Then create experiment directory 

```bash
EXPERIMENT_NAME=$(date | tr " " "_")
GOLDFISH_DIRECTORY=${DATASET_PATH?}/friday/goldfish/${EXPERIMENT_NAME?}
mkdir -p $GOLDFISH_DIRECTORY
```

The following script then converts it into Goldfish voice format

```bash
bazel run //friday/audio/keyword_detection/goldfish/preprocessing:google_speech_commands_to_goldfish_voice --\
  --base_path=${DATASET_PATH?}/google_speech_commands \
  --output_prefix=${GOLDFISH_DIRECTORY?}/tfexamples.speech_commands\
  --sample_rate=8000
```

### Recording your own

This assumes GOLDFISH_DIRECTORY is set, replace --text with the text you will be saying.

Once the program starts, you press enter then you have 'clip_length' e.g 2 seconds time to speak the text. After you have spoken it will be
repeated to you. Then press enter when you're ready to speak the same text again and repeat. Each time you press enter it will record for 2 seconds then play it back.
Once you're done, do one keyboard interrupt and it will save your recordings into a format that can be run in the [Pipeline](#pipeline).

```bash
TEXT="godmorgon"

bazel run //friday/audio/keyword_detection/goldfish/preprocessing:record_personal_examples --\
    --file_prefix=${GOLDFISH_DIRECTORY?}/tfexamples\
    --sample_rate=8000\
    --clip_length=2.0\
    --text=${TEXT?}

```

Recording background noise
```bash
bazel run //friday/audio/keyword_detection/goldfish/preprocessing:record_personal_examples --\
    --file_prefix=${GOLDFISH_DIRECTORY?}/tfexamples\
    --sample_rate=8000\
    --clip_length=2.0\
    --text=""\
    --background="[UNK]"
```

### Manually filtering a record

If you when for example recorded your own data made a mistake and said the wrong word, you can use this to
drop examples from the record.

```bash
INPUT_FILE=...
OUTPUT_FILE=...

bazel run //friday/audio/keyword_detection/goldfish/preprocessing:manually_filter_records -- \
 --input_file=${GOLDFISH_DIRECTORY?}/${INPUT_FILE} \
 --output_file=${GOLDFISH_DIRECTORY?}/${OUTPUT_FILE}
```

### Cleaning Goldfish Voice data

#### Pipeline
The pipeline 'preprocess_goldfish.py' prepares the dataset for training, it adds labels amongst other things.

```bash
LABEL_MAP_PATH=${PWD}/friday/audio/keyword_detection/goldfish/configs/google_speech_commands_label_map.json
LABEL_MAP_PATH=${PWD}/friday/audio/keyword_detection/goldfish/configs/google_speech_commands_few_label_map.json
LABEL_MAP_PATH=${PWD}/friday/audio/keyword_detection/goldfish/configs/tänd_släck.json
LABEL_MAP_PATH=${PWD}/friday/audio/keyword_detection/goldfish/configs/kombination.json


bazel run //friday/audio/keyword_detection/goldfish/preprocessing:preprocess_goldfish --\
 "--source_prefix=${GOLDFISH_DIRECTORY?}/tfexamples*"\
  --output_path=${GOLDFISH_DIRECTORY?}/ptfexamples\
  --label_map_path=${LABEL_MAP_PATH?}\
  --maximum_clip_length=2\
  --in_memory_files=20
```

After this pipeline run the splitting

#### Splitting

The train_valid_split pipeline creates one train and one validation split.

```bash
bazel run //friday/audio/keyword_detection/goldfish/preprocessing:train_valid_split --\
 "--source_prefix=${GOLDFISH_DIRECTORY?}/ptfexamples*"\
 --sink_prefix=${GOLDFISH_DIRECTORY?}/ptfexamples\
 --examples_per_shard=100\
 --train_fraction=0.8
```

Once this is done you can train a model, see e.g [magikarp](https://github.com/JonasRSV/friday-voice-assistant/blob/master/friday/audio/keyword_detection/goldfish/docs/model_magikarp.md)
