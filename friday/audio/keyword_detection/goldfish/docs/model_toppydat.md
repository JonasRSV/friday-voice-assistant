# Toppydat


Toppydat is a TDA approach to differentiating between raw audio signals. 



Run a demo using

```bash
bazel run //friday/audio/keyword_detection/goldfish/models/toppydat:toppydat --\
    --positive=$DATASET_PATH/friday/goldfish/tand_slack/tfexamples.personal.168106\
    --negative=$DATASET_PATH/friday/goldfish/tand_slack/tfexamples.personal.2370248\
    --neutral=$DATASET_PATH/friday/goldfish/tand_slack/tfexamples.personal.1708282
```
