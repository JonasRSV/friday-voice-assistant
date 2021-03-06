
# Magikarp

This document explains how to train the Magikarp keyword spotting model. First set the
model output directory

```bash
EXPERIMENT_NAME=magikarp.$(date | tr " " "_")
MODEL_OUTPUT=/tmp/$EXPERIMENT_NAME
```

Then for training launch

```bash
bazel run //friday/audio/keyword_detection/goldfish/models/magikarp:magikarp --\
    "--train_prefix=$GOLDFISH_DIRECTORY/ptfexamples.train*"\
    "--eval_prefix=$GOLDFISH_DIRECTORY/ptfexamples.valid*"\
    --model_directory=$MODEL_OUTPUT\
    --mode="train_eval"\
    --sample_rate=8000\
    --clip_length=2.0\
    --batch_size=32\
    --start_learning_rate=0.001\
    --learning_rate_decay=0.90\
    --learning_decay_steps=500\
    --num_labels=10\
    --max_steps=1000000\
    --save_summary_every=500\
    --eval_every=500\
    --parallel_reads=5
```

***If you get obscure errors***
- Are --num_labels correct? Is it the same as in the label map used to create your data?


To Export after training run
```bash
bazel run //friday/audio/keyword_detection/goldfish/models/magikarp:magikarp --\
    --model_directory=$MODEL_OUTPUT\
    --mode="export"\
    --sample_rate=8000\
    --clip_length=2.0\
    --num_labels=10
    
```

***If you get obscure errors***
- Are --num_labels correct? Is it the same as in the label map used to create your data?
