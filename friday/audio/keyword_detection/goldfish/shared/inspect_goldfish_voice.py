import simpleaudio
import tensorflow as tf
import argparse
import pathlib
import numpy as np
import matplotlib.pyplot as plt
from enum import Enum

import friday.audio.keyword_detection.goldfish.shared.goldfish_utils as utils

tf.compat.v1.enable_eager_execution()


class InvalidFileError(Exception):
    """"""
    pass


class Mode(Enum):
    play_audio = "play_audio"
    visualize = "visualize"
    meta = "meta"


def play_audio(file: str):
    # TODO(jonasrsv): support for sharding and advanced choice

    file_path = pathlib.Path(file)

    if not file_path.is_file():
        raise InvalidFileError(f"{file} is not a valid file")

    dataset = tf.data.TFRecordDataset([file])
    for serialized_example in dataset.take(10):
        example = tf.train.Example()
        example.ParseFromString(serialized_example.numpy())

        print("Playing", utils.get_text(example))

        audio = np.array(utils.get_audio(example), dtype=np.int16)
        sample_rate = utils.get_sample_rate(example)

        simpleaudio.play_buffer(audio, 1, 2,
                                sample_rate=sample_rate).wait_done()


def visualize(file: str):
    file_path = pathlib.Path(file)

    if not file_path.is_file():
        raise InvalidFileError(f"{file} is not a valid file")

    dataset = tf.data.TFRecordDataset([file])
    for i, serialized_example in enumerate(dataset.take(5), 1):
        example = tf.train.Example()
        example.ParseFromString(serialized_example.numpy())

        audio = np.array(utils.get_audio(example), dtype=np.int16)
        text = utils.get_text(example)

        plt.subplot(5, 1, i)
        plt.title(f"{text}")
        x = np.arange(audio.size)
        y = audio
        plt.plot(x, y)

    plt.tight_layout()
    plt.show()


def show_meta(file: str):
    file_path = pathlib.Path(file)

    if not file_path.is_file():
        raise InvalidFileError(f"{file} is not a valid file")

    dataset = tf.data.TFRecordDataset([file])
    for i, serialized_example in enumerate(dataset, 1):
        example = tf.train.Example()
        example.ParseFromString(serialized_example.numpy())

        text = utils.get_text(example)
        sample_rate = utils.get_sample_rate(example)

        label = None
        try:
            label = utils.get_label(example)
        except IndexError:
            pass

        print(f"text: {text}\nsample_rate: {sample_rate}\nlabel: {label}\n\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--file",
                        type=str,
                        help="Path to goldfish audio file",
                        required=True)
    parser.add_argument("--mode", type=Mode, choices=list(Mode), required=True)

    args = parser.parse_args()

    mode = {Mode.play_audio: play_audio,
            Mode.visualize: visualize,
            Mode.meta: show_meta}

    mode[args.mode](args.file)
