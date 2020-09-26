# Docs for shared tools

- [Inspect goldfish voice](#inspect-goldfish-voice)


## Inspect Goldfish Voice

---

To inspect that the tfexamples contains valid voice data one can use e.g

```bash
bazel run //friday/audio/keyword_detection/goldfish/shared:inspect_goldfish_voice --\
 --file=$GOLDFISH_DIRECTORY/tfexamples-TIMESTAMP-ID \
 --mode=play_audio
```

To inspect sound waves of the tfexamples use

```bash 
bazel run //friday/audio/keyword_detection/goldfish/shared:inspect_goldfish_voice --\
  --file=$GOLDFISH_DIRECTORY/tfexamples-TIMESTAMP-ID --mode=visualize
```

To show just meta information use

```bash 
bazel run //friday/audio/keyword_detection/goldfish/shared:inspect_goldfish_voice --\
  --file=$GOLDFISH_DIRECTORY/tfexamples-TIMESTAMP-ID --mode=meta
```
