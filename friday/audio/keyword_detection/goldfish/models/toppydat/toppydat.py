import friday.audio.keyword_detection.goldfish.shared.goldfish_utils as utils
import os
import argparse
import matplotlib.patches as mpatches
import numpy as np
import random
import seaborn as sb
import tensorflow as tf
import matplotlib.pyplot as plt
import librosa
import librosa.display

tf.compat.v1.enable_eager_execution()


def _load_data(path: str) -> np.ndarray:
    cls = ""
    numpy_arrays = []
    for example in tf.data.TFRecordDataset(filenames=[path]):
        example = example.numpy()
        example = tf.train.Example.FromString(example)

        numpy_arrays.append(utils.get_audio(example))
        cls = utils.get_text(example)

    print(f"{path} contains {cls}")

    return np.array(numpy_arrays)


class Mode:
    introduction_examples = 0
    overview = 1


def create_plot(positive_signals: np.ndarray, negative_signals: np.ndarray,
                neutral_signals: np.ndarray, mode: int):
    x = np.arange(positive_signals[0].size)

    if mode == Mode.overview:
        plt.figure(figsize=(10, 10))
        for i in range(3):
            for j, signal in enumerate([positive_signals, negative_signals, neutral_signals], 1):
                plt.subplot(3, 3, 3 * i + j)
                plt.yticks([])
                plt.xticks([])
                plt.ylim([-32000, 32000])

                plt.plot(x, signal[i])
    elif mode == Mode.introduction_examples:
        sb.set_theme(style="dark")
        plt.figure(figsize=(10, 5))
        positive_signal = random.choice(positive_signals)
        negative_signal = random.choice(negative_signals)

        plt.yticks([])
        plt.xticks(fontsize=18)
        plt.ylim([-32000, 32000])
        sb.scatterplot(x, positive_signal + 8000, size=1, alpha=0.6, color="#0088FF")
        sb.scatterplot(x, negative_signal - 8000, size=1, alpha=0.6, color="#FF8400")

        red_patch = mpatches.Patch(color='#0088FF', label='Tänd Ljuset')
        blue_patch = mpatches.Patch(color='#FF8400', label='Släck Ljuset')
        plt.legend(handles=[red_patch, blue_patch], fontsize=18)

        plt.savefig(f"{os.environ['TOPDAT']}/images/introduction_signal_example.png", bbox_inches="tight")

        plt.figure(figsize=(10, 5))
        plt.yticks([])
        plt.xticks(fontsize=18)
        plt.ylim([-32000, 32000])

        plt.subplot(2, 1, 1)
        plt.title("Tänd Ljuset", fontsize=18)
        positive_spectrogram = librosa.feature.melspectrogram(y=positive_signal / 32768.0, sr=8000)

        librosa.display.specshow(librosa.power_to_db(positive_spectrogram, ref=np.max))
        plt.subplot(2, 1, 2)
        plt.title("Släck Ljuset", fontsize=18)

        librosa.display.specshow(librosa.power_to_db(negative_spectrogram, ref=np.max))

        plt.savefig(f"{os.environ['TOPDAT']}/images/introduction_spectrogram_example.png", bbox_inches="tight")

    plt.show()


def main(positive_examples_path: str, negative_examples_path: str,
         neutral_examples_path: str):
    positive_examples = _load_data(positive_examples_path)
    negative_examples = _load_data(negative_examples_path)
    neutral_examples = _load_data(neutral_examples_path)

    create_plot(positive_examples, negative_examples, neutral_examples, Mode.introduction_examples)


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
