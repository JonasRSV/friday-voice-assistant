
# Golding

This document explains how to train the Golding keyword spotting model. First set the
model output directory

```bash
EXPERIMENT_NAME=golding.$(date | tr " " "_")
MODEL_OUTPUT=/tmp/$EXPERIMENT_NAME
```

Then for training launch

```bash
bazel run //friday/audio/keyword_detection/goldfish/models/golding:golding --\
    "--train_prefix=$GOLDFISH_DIRECTORY/ptfexamples.train*"\
    "--eval_prefix=$GOLDFISH_DIRECTORY/ptfexamples.valid*"\
    --model_directory=$MODEL_OUTPUT\
    --mode="train_eval"\
    --sample_rate=8000\
    --clip_length=2.0\
    --batch_size=64\
    --start_learning_rate=0.01\
    --learning_rate_decay=0.95\
    --learning_decay_steps=1000\
    --num_labels=3\
    --max_steps=1000000\
    --save_summary_every=500\
    --eval_every=500\
    --parallel_reads=5
```

To Export after training run
```bash
bazel run //friday/audio/keyword_detection/goldfish/models/golding:golding --\
    --model_directory=$MODEL_OUTPUT\
    --mode="export"\
    --sample_rate=8000\
    --clip_length=2.0\
    --num_labels=3
    
```
