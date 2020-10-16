import friday.audio.keyword_detection.goldfish.shared.goldfish_utils as utils
import os
import time
import sys
import argparse
from collections import Counter
from tqdm import tqdm
import matplotlib.patches as mpatches
import numpy as np
import random
import seaborn as sb
import tensorflow as tf
import matplotlib.pyplot as plt
import librosa
import librosa.display
import ripser
import numba

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
        #sb.set_theme(style="dark")
        plt.figure(figsize=(10, 5))
        positive_signal = random.choice(positive_signals)
        negative_signal = random.choice(negative_signals)

        plt.yticks([])
        plt.xticks(fontsize=18)
        plt.ylim([-32000, 32000])
        plt.axis("off")
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

        negative_spectrogram = librosa.feature.melspectrogram(y=negative_signal / 32768.0, sr=8000)
        librosa.display.specshow(librosa.power_to_db(negative_spectrogram, ref=np.max))

        plt.savefig(f"{os.environ['TOPDAT']}/images/introduction_spectrogram_example.png", bbox_inches="tight")

    plt.show()


def _get_spectrogram(signal: np.ndarray):
    return librosa.feature.melspectrogram(y=signal / 32768.0, sr=8000)


@numba.jit(nopython=True, forceobj=False)
def _spectrogram_to_points(spectrogram: np.ndarray):
    heigth, width = spectrogram.shape

    points = []
    for i in range(heigth):
        for j in range(width):
            points.append([float(i), float(j), spectrogram[i, j]])

    return np.array(points)


@numba.jit(nopython=True, forceobj=False)
def euclidean_distance(x, y):
    return np.sqrt(np.sum(np.square(x - y)))


@numba.jit(nopython=True, forceobj=False)
def cosine_distance(x, y):
    return 1 - x @ y / (np.sqrt(x @ x) * np.sqrt(y @ y))


@numba.jit(nopython=True, forceobj=False)
def chebyshev_distance(x, y):
    return np.max(np.abs(x - y))


@numba.jit(nopython=True, forceobj=False)
def _get_distance_matrix(point_cloud: np.ndarray, distance_fn):
    n_points = len(point_cloud)

    distance_matrix = np.zeros((n_points, n_points))
    for i in range(n_points):
        for j in range(n_points):
            distance_matrix[i, j] = distance_fn(point_cloud[i], point_cloud[j])

    return distance_matrix


@numba.jit(nopython=True, forceobj=False)
def _get_pcf(homology, t_range):
    bar_sizes = np.abs(homology[:, 0] - homology[:, 1])
    pcf = []

    for i in t_range:
        pcf.append(np.sum(bar_sizes > i))

    return pcf


def _get_signature(point_cloud, metric_fn, n_homology: int, t_ranges):
    distance_matrix = _get_distance_matrix(point_cloud, metric_fn)

    rips = ripser.Rips(maxdim=n_homology, verbose=False)

    # This is a list of arrays of birth's and deaths for h_0, .. h_maxdim
    homologies = rips.fit_transform(distance_matrix, distance_matrix=True)

    # Turn each presistence diagram into a PCF
    return [_get_pcf(homology, t_range) for homology, t_range in zip(homologies, t_ranges)]


def _sub_sample_point_cloud_based_on_intensity(point_cloud: np.ndarray, n_samples: int, n_times: int):
    # Yes this entire function could be replaced by picking based on sorting on probability
    p = point_cloud[:, 2]

    p = np.abs(p)
    p = p / p.sum()

    indexes = np.arange(len(point_cloud))

    choices = np.random.choice(indexes, size=n_samples * n_times, p=p)

    counts = Counter(choices)
    top_n = counts.most_common(n_samples)
    indexes = np.array([index for (index, count) in top_n[:n_samples]])

    return point_cloud[indexes]


def euclidean_distance_plots(positive_point_clouds: np.ndarray, negative_point_clouds: np.ndarray):
    print("Positive", len(positive_point_clouds), "negative", len(negative_point_clouds))
    n_homology = 3

    t_ranges = [
        np.linspace(0, 50, 1000),
        np.linspace(0, 5, 1000),
        np.linspace(0, 1.0, 1000),
    ]
    timestamp = time.time()
    positive_signatures = [_get_signature(sub_point_cloud,
                                          euclidean_distance,
                                          n_homology=n_homology - 1,
                                          t_ranges=t_ranges)
                           for sub_point_cloud in positive_point_clouds[:50]]
    negative_signatures = [_get_signature(sub_point_cloud,
                                          euclidean_distance,
                                          n_homology=n_homology - 1,
                                          t_ranges=t_ranges)
                           for sub_point_cloud in negative_point_clouds[:50]]
    print(f"Getting signatures took {time.time() - timestamp}")

    titles = ["H0", "H1", "H2"]
    colors = ["blue", "orange"]

    def plot_signatures():
        plt.figure(figsize=(10, 6))
        plt.suptitle("Euclidean Metric", fontsize=20)
        for i in range(n_homology):
            plt.subplot(n_homology, 1, i + 1)
            plt.title(titles[i], fontsize=16)

            t_range = t_ranges[i]

            #for signature in positive_signatures:
            #    sb.lineplot(t_range, signature[i], color=colors[0])

            #for signature in negative_signatures:
            #    sb.lineplot(t_range, signature[i], color=colors[1])

            def plot_distplot(signatures, color):
               print(signatures.shape)
               mean = signatures.mean(axis=0)
               std = signatures.std(axis=0)

               sb.lineplot(t_range, mean, color=color)
               plt.fill_between(t_range, mean - std, mean + std, color=color, alpha=0.3)

            signatures = np.array([signature[i] for signature in positive_signatures])
            plot_distplot(signatures, color=colors[0])

            signatures = np.array([signature[i] for signature in negative_signatures])
            plot_distplot(signatures, color=colors[1])

        plt.savefig(f"{os.environ['TOPDAT']}/images/average_euclidean_metric.png", bbox_inches="tight")
        plt.show()

    def plot_confusion_matrix():
        plt.figure(figsize=(10, 6))
        plt.suptitle("Euclidean Metric", fontsize=20)
        for i in range(n_homology):
            plt.subplot(n_homology, 1, i + 1)
            plt.title(titles[i], fontsize=16)

            pos_signatures = np.array([signature[i] for signature in positive_signatures])
            positive_mean = pos_signatures.mean(axis=0)

            neg_signatures = np.array([signature[i] for signature in negative_signatures])
            negative_mean = neg_signatures.mean(axis=0)

            pos_to_pos_mean = np.sqrt(np.square(pos_signatures - positive_mean).sum(axis=1))
            pos_to_neg_mean = np.sqrt(np.square(pos_signatures - negative_mean).sum(axis=1))

            neg_to_pos_mean = np.sqrt(np.square(neg_signatures - positive_mean).sum(axis=1))
            neg_to_neg_mean = np.sqrt(np.square(neg_signatures - negative_mean).sum(axis=1))

            confusion_matrix = np.zeros((2, 2), dtype=np.int32)
            # True positives
            confusion_matrix[0, 0] = np.sum(pos_to_pos_mean < pos_to_neg_mean)

            # False negatives
            confusion_matrix[1, 0] = np.sum(pos_to_pos_mean >= pos_to_neg_mean)

            # True negatives
            confusion_matrix[1, 1] = np.sum(neg_to_neg_mean < neg_to_pos_mean)

            # False positives
            confusion_matrix[0, 1] = np.sum(neg_to_neg_mean >= neg_to_pos_mean)

            sb.heatmap(confusion_matrix, annot=True, fmt="d")

        plt.savefig(f"{os.environ['TOPDAT']}/images/euclidean_heatmaps.png", bbox_inches="tight")
        plt.show()

    #plot_signatures()
    plot_confusion_matrix()


def cosine_distance_plots(positive_point_clouds: np.ndarray, negative_point_clouds: np.ndarray):
    print("Positive", len(positive_point_clouds), "negative", len(negative_point_clouds))
    n_homology = 3

    t_ranges = [
        np.linspace(0, 0.5, 1000),
        np.linspace(0, 0.05, 1000),
        np.linspace(0, 0.15, 1000),
        np.linspace(0, 0.1, 1000),
    ]

    y_max = [
        50,
        400,
        1000,
        10000,
    ]
    timestamp = time.time()
    with tqdm(total=len(positive_point_clouds) + len(negative_point_clouds)) as progress_bar:
        positive_signatures = []
        for signature in (_get_signature(sub_point_cloud,
                                         cosine_distance,
                                         n_homology=n_homology - 1,
                                         t_ranges=t_ranges)
                          for sub_point_cloud in positive_point_clouds[:10]):
            positive_signatures.append(signature)
            progress_bar.update()

        negative_signatures = []
        for signature in (_get_signature(sub_point_cloud,
                                         cosine_distance,
                                         n_homology=n_homology - 1,
                                         t_ranges=t_ranges)
                          for sub_point_cloud in negative_point_clouds[:10]):
            negative_signatures.append(signature)
            progress_bar.update()

    print(f"Getting signatures took {time.time() - timestamp}")

    titles = ["H0", "H1", "H2", "H3"]
    colors = ["blue", "orange"]

    def plot_signatures():
        plt.figure(figsize=(10, 6))
        plt.suptitle("Cosine Similarity", fontsize=20)
        for i in range(n_homology):
            plt.subplot(n_homology, 1, i + 1)
            plt.title(titles[i], fontsize=16)

            #plt.ylim([0, y_max[i]])

            t_range = t_ranges[i]

            #for signature in positive_signatures:
            #    sb.lineplot(t_range, signature[i], color=colors[0])

            #for signature in negative_signatures:
            #    sb.lineplot(t_range, signature[i], color=colors[1])

            def plot_distplot(signatures, color):
               mean = signatures.mean(axis=0)
               std = signatures.std(axis=0)

               sb.lineplot(t_range, mean, color=color)
               plt.fill_between(t_range, mean - std, mean + std, color=color, alpha=0.3)

            signatures = np.array([signature[i] for signature in positive_signatures])
            plot_distplot(signatures, color=colors[0])

            signatures = np.array([signature[i] for signature in negative_signatures])
            plot_distplot(signatures, color=colors[1])

        plt.savefig(f"{os.environ['TOPDAT']}/images/average_cosine_similarity.png", bbox_inches="tight")
        plt.show()

    def plot_confusion_matrix():
        plt.figure(figsize=(10, 6))
        plt.suptitle("Cosine Similarity", fontsize=20)
        for i in range(n_homology):
            plt.subplot(n_homology, 1, i + 1)
            plt.title(titles[i], fontsize=16)

            pos_signatures = np.array([signature[i] for signature in positive_signatures])
            positive_mean = pos_signatures.mean(axis=0)

            neg_signatures = np.array([signature[i] for signature in negative_signatures])
            negative_mean = neg_signatures.mean(axis=0)

            pos_to_pos_mean = np.sqrt(np.square(pos_signatures - positive_mean).sum(axis=1))
            pos_to_neg_mean = np.sqrt(np.square(pos_signatures - negative_mean).sum(axis=1))

            neg_to_pos_mean = np.sqrt(np.square(neg_signatures - positive_mean).sum(axis=1))
            neg_to_neg_mean = np.sqrt(np.square(neg_signatures - negative_mean).sum(axis=1))

            confusion_matrix = np.zeros((2, 2), dtype=np.int32)
            # True positives
            confusion_matrix[0, 0] = np.sum(pos_to_pos_mean < pos_to_neg_mean)

            # False negatives
            confusion_matrix[1, 0] = np.sum(pos_to_pos_mean >= pos_to_neg_mean)

            # True negatives
            confusion_matrix[1, 1] = np.sum(neg_to_neg_mean < neg_to_pos_mean)

            # False positives
            confusion_matrix[0, 1] = np.sum(neg_to_neg_mean >= neg_to_pos_mean)

            sb.heatmap(confusion_matrix, annot=True, fmt="d")

        plt.savefig(f"{os.environ['TOPDAT']}/images/cosine_heatmaps.png", bbox_inches="tight")
        plt.show()

    plot_signatures()
    #plot_confusion_matrix()


def chebyshev_plots(positive_point_clouds: np.ndarray, negative_point_clouds: np.ndarray):
    print("Positive", len(positive_point_clouds), "negative", len(negative_point_clouds))
    n_homology = 3

    t_ranges = [
        np.linspace(0, 100, 1000),
        np.linspace(0, 5.0, 1000),
        np.linspace(0, 5.0, 1000),
        np.linspace(0, 0.1, 1000),
    ]

    y_max = [
        50,
        5,
        5,
        2,
    ]
    timestamp = time.time()
    with tqdm(total=len(positive_point_clouds) + len(negative_point_clouds)) as progress_bar:
        positive_signatures = []
        for signature in (_get_signature(sub_point_cloud,
                                         chebyshev_distance,
                                         n_homology=n_homology - 1,
                                         t_ranges=t_ranges)
                          for sub_point_cloud in positive_point_clouds[:10]):
            positive_signatures.append(signature)
            progress_bar.update()

        negative_signatures = []
        for signature in (_get_signature(sub_point_cloud,
                                         chebyshev_distance,
                                         n_homology=n_homology - 1,
                                         t_ranges=t_ranges)
                          for sub_point_cloud in negative_point_clouds[:10]):
            negative_signatures.append(signature)
            progress_bar.update()

    print(f"Getting signatures took {time.time() - timestamp}")

    titles = ["H0", "H1", "H2", "H3"]
    colors = ["blue", "orange"]

    def plot_signatures():
        plt.figure(figsize=(10, 6))
        plt.suptitle("Chebyshev distance", fontsize=20)
        for i in range(n_homology):
            plt.subplot(n_homology, 1, i + 1)
            plt.title(titles[i], fontsize=16)

            # plt.ylim([0, y_max[i]])

            t_range = t_ranges[i]

            for signature in positive_signatures:
                sb.lineplot(t_range, signature[i], color=colors[0])

            for signature in negative_signatures:
                sb.lineplot(t_range, signature[i], color=colors[1])

            #def plot_distplot(signatures, color):
            #  mean = signatures.mean(axis=0)
            #  std = signatures.std(axis=0)

            #  sb.lineplot(t_range, mean, color=color)
            #  plt.fill_between(t_range, mean - std, mean + std, color=color, alpha=0.3)

            #signatures = np.array([signature[i] for signature in positive_signatures])
            #plot_distplot(signatures, color=colors[0])

            #signatures = np.array([signature[i] for signature in negative_signatures])
            #plot_distplot(signatures, color=colors[1])

        plt.savefig(f"{os.environ['TOPDAT']}/images/chebyshev_distance.png", bbox_inches="tight")
        plt.show()

    def plot_confusion_matrix():
        plt.figure(figsize=(10, 6))
        plt.suptitle("Chebyshev Distance", fontsize=20)
        for i in range(n_homology):
            plt.subplot(n_homology, 1, i + 1)
            plt.title(titles[i], fontsize=16)

            pos_signatures = np.array([signature[i] for signature in positive_signatures])
            positive_mean = pos_signatures.mean(axis=0)

            neg_signatures = np.array([signature[i] for signature in negative_signatures])
            negative_mean = neg_signatures.mean(axis=0)

            pos_to_pos_mean = np.sqrt(np.square(pos_signatures - positive_mean).sum(axis=1))
            pos_to_neg_mean = np.sqrt(np.square(pos_signatures - negative_mean).sum(axis=1))

            neg_to_pos_mean = np.sqrt(np.square(neg_signatures - positive_mean).sum(axis=1))
            neg_to_neg_mean = np.sqrt(np.square(neg_signatures - negative_mean).sum(axis=1))

            confusion_matrix = np.zeros((2, 2), dtype=np.int32)
            # True positives
            confusion_matrix[0, 0] = np.sum(pos_to_pos_mean < pos_to_neg_mean)

            # False negatives
            confusion_matrix[1, 0] = np.sum(pos_to_pos_mean >= pos_to_neg_mean)

            # True negatives
            confusion_matrix[1, 1] = np.sum(neg_to_neg_mean < neg_to_pos_mean)

            # False positives
            confusion_matrix[0, 1] = np.sum(neg_to_neg_mean >= neg_to_pos_mean)

            sb.heatmap(confusion_matrix, annot=True, fmt="d")

        plt.savefig(f"{os.environ['TOPDAT']}/images/chebyshev_heatmaps.png", bbox_inches="tight")
        plt.show()

    plot_signatures()
    #plot_confusion_matrix()


def heatmap_of_raw_spectrograms(positive_spectrograms, negative_spectrograms):
    plt.figure(figsize=(10, 6))
    plt.suptitle("Spectrograms ", fontsize=20)

    positive_spectrograms = positive_spectrograms.reshape(50, -1)
    negative_spectrograms = negative_spectrograms.reshape(50, -1)

    positive_mean = positive_spectrograms.mean(axis=0)
    negative_mean = negative_spectrograms.mean(axis=0)

    pos_to_pos_mean = np.sqrt(np.square(positive_spectrograms - positive_mean).sum(axis=1))
    pos_to_neg_mean = np.sqrt(np.square(positive_spectrograms - negative_mean).sum(axis=1))

    neg_to_pos_mean = np.sqrt(np.square(negative_spectrograms - positive_mean).sum(axis=1))
    neg_to_neg_mean = np.sqrt(np.square(negative_spectrograms - negative_mean).sum(axis=1))

    confusion_matrix = np.zeros((2, 2), dtype=np.int32)
    # True positives
    confusion_matrix[0, 0] = np.sum(pos_to_pos_mean < pos_to_neg_mean)

    # False negatives
    confusion_matrix[1, 0] = np.sum(pos_to_pos_mean >= pos_to_neg_mean)

    # True negatives
    confusion_matrix[1, 1] = np.sum(neg_to_neg_mean < neg_to_pos_mean)

    # False positives
    confusion_matrix[0, 1] = np.sum(neg_to_neg_mean >= neg_to_pos_mean)

    sb.heatmap(confusion_matrix, annot=True, fmt="d")

    plt.savefig(f"{os.environ['TOPDAT']}/images/spectrogram_heatmaps.png", bbox_inches="tight")
    plt.show()


def main(positive_examples_path: str, negative_examples_path: str,
         neutral_examples_path: str):
    positive_examples = _load_data(positive_examples_path)
    negative_examples = _load_data(negative_examples_path)
    neutral_examples = _load_data(neutral_examples_path)

    # Creates plots for introduction
    # create_plot(positive_examples, negative_examples, neutral_examples, Mode.introduction_examples)

    random.shuffle(positive_examples)
    random.shuffle(neutral_examples)

    timestamp = time.time()
    positive_spectrograms = np.array([_get_spectrogram(signal) for signal in positive_examples])
    negative_spectrograms = np.array([_get_spectrogram(signal) for signal in negative_examples])
    print(f"Creating spectrograms took {time.time() - timestamp}")

    # This is our space
    timestamp = time.time()
    positive_point_clouds = np.array([_spectrogram_to_points(spectrogram) for spectrogram in positive_spectrograms])
    negative_point_clouds = np.array([_spectrogram_to_points(spectrogram) for spectrogram in negative_spectrograms])
    print(f"Creating point-clouds took {time.time() - timestamp}")

    # Number of times to sub-sample and number of samples
    n_times, samples = 1000, 200

    timestamp = time.time()
    positive_sub_point_clouds = np.array([
        _sub_sample_point_cloud_based_on_intensity(point_cloud, samples, n_times=n_times)
        for point_cloud in positive_point_clouds
    ])

    negative_sub_point_clouds = np.array([
        _sub_sample_point_cloud_based_on_intensity(point_cloud, samples, n_times=n_times)
        for point_cloud in negative_point_clouds
    ])
    print(f"subsampling point-clouds took {time.time() - timestamp}")

    # euclidean_distance_plots(positive_sub_point_clouds, negative_sub_point_clouds)
    # cosine_distance_plots(positive_sub_point_clouds, negative_sub_point_clouds)
    #chebyshev_plots(positive_sub_point_clouds, negative_sub_point_clouds)
    heatmap_of_raw_spectrograms(positive_sub_point_clouds[:50], negative_sub_point_clouds[:50])


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
