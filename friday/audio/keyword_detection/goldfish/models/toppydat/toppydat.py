import friday.audio.keyword_detection.goldfish.shared.goldfish_utils as utils
import logging
import argparse
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from pathlib import Path
from typing import List

tf.compat.v1.enable_eager_execution()


def _load_data(path: str) -> np.ndarray:
    numpy_arrays = []
    for example in tf.data.TFRecordDataset(filenames=[path]):
        example = example.numpy()
        example = tf.train.Example.FromString(example)

        numpy_arrays.append(utils.get_audio(example))

    return np.array(numpy_arrays)


def visualize(positive_signals: np.ndarray, negative_signals: np.ndarray,
              neutral_signals: np.ndarray):

    x = np.arange(positive_signals[0].size)

    plt.figure(figsize=(10, 10))
    for i in range(3):
        for j, signal in enumerate([positive_signals, negative_signals, neutral_signals], 1):
            plt.subplot(3, 3, 3 * i + j)
            plt.yticks([])
            plt.xticks([])
            plt.ylim([-32000, 32000])

            plt.plot(x, signal[i])


    plt.show()


def main(positive_examples_path: str, negative_examples_path: str,
         neutral_examples_path: str):

    positive_examples = _load_data(positive_examples_path)
    negative_examples = _load_data(negative_examples_path)
    neutral_examples = _load_data(neutral_examples_path)

    visualize(positive_examples, negative_examples, neutral_examples)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--positive",
                        type=str,
                        help="Path to positive classes",
                        required=True)
    parser.add_argument("--negative",
                        type=str,
                        help="Path to negative classes",
                        required=True)
    parser.add_argument("--neutral",
                        type=str,
                        help="Path to neutral classes",
                        required=True)

    args = parser.parse_args()
    main(positive_examples_path=args.positive,
         negative_examples_path=args.negative,
         neutral_examples_path=args.neutral)
