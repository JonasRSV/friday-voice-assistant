import pathlib
import tensorflow as tf
import friday.audio.keyword_detection.goldfish.models.shared.audio as audio
import argparse
import sys
from enum import Enum

tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.INFO)

AUDIO_SHAPE = None


class Mode(Enum):
    train_eval = "train_eval"
    export = "export"


def create_input_fn(mode: tf.estimator.ModeKeys,
                    goldfish_directory: str,
                    parallel_reads: int = 5,
                    batch_size: int = 32):
    feature_description = {
        'label': tf.io.FixedLenFeature([], tf.int64),
        'audio': tf.io.FixedLenFeature([AUDIO_SHAPE], tf.int64),
    }

    def decode_example(x):
        return tf.io.parse_single_example(x, feature_description)

    def cast_to_int16(x):
        x["audio"] = tf.cast(x["audio"], tf.int16)
        return x

    def input_fn():
        files = [
            str(x)
            for x in pathlib.Path(goldfish_directory).glob("ptfexample*")
        ]

        dataset = tf.data.TFRecordDataset(filenames=files,
                                          num_parallel_reads=parallel_reads)
        dataset = dataset.map(decode_example)
        dataset = dataset.map(cast_to_int16)

        if mode == tf.estimator.ModeKeys.TRAIN:
            dataset = dataset.shuffle(buffer_size=100)

        dataset = dataset.batch(batch_size=batch_size)

        if mode == tf.estimator.ModeKeys.TRAIN:
            dataset = dataset.repeat(100000000)

        return dataset

    return input_fn


def raw_audio_model(signal: tf.Tensor, num_labels: int) -> tf.Tensor:
    """A convolution based model.

    Args:
        signal: Audio signal scaled to [-1, 1]
    Returns:
        Logits
    """
    x = tf.expand_dims(signal, -1)
    x = tf.compat.v1.layers.Conv1D(filters=64,
                                   kernel_size=7,
                                   strides=2,
                                   activation=tf.nn.relu)(x)
    x = tf.compat.v1.layers.MaxPooling1D(pool_size=7, strides=5)(x)
    x = tf.compat.v1.layers.Conv1D(filters=64,
                                   kernel_size=5,
                                   strides=2,
                                   activation=tf.nn.relu)(x)
    x = tf.compat.v1.layers.MaxPooling1D(pool_size=3, strides=2)(x)
    x = tf.compat.v1.layers.Conv1D(filters=64,
                                   kernel_size=3,
                                   strides=2,
                                   activation=tf.nn.relu)(x)
    x = tf.compat.v1.layers.MaxPooling1D(pool_size=3, strides=2)(x)
    x = tf.compat.v1.layers.Conv1D(filters=64,
                                   kernel_size=3,
                                   activation=tf.nn.relu)(x)
    x = tf.compat.v1.layers.MaxPooling1D(pool_size=3, strides=2)(x)
    print("X\n\n\n\n\n\n\n", x)
    x = tf.compat.v1.layers.Flatten()(x)
    print("X\n\n\n\n\n\n\n", x)
    x = tf.compat.v1.layers.Dense(num_labels, activation=None)(x)

    return x


def make_model_fn(summary_output_dir: str,
                  num_labels: int,
                  sample_rate: int = 44100,
                  save_summaries_every: int = 100):
    def model_fn(features, labels, mode, config, params):
        audio_signal = features["audio"]

        # Expand single prediction to batch
        if mode == tf.estimator.ModeKeys.PREDICT:
            audio_signal = tf.expand_dims(audio_signal, 0)

        # Normalize audio to [-1, 1]
        signal = audio.normalize(audio_signal)

        if mode != tf.estimator.ModeKeys.PREDICT:
            tf.summary.audio(name="audio",
                             tensor=signal,
                             sample_rate=sample_rate)

        logits = raw_audio_model(signal=signal, num_labels=num_labels)

        predict_op = tf.argmax(logits, axis=1)

        loss_op, train_op, train_logging_hooks, eval_metric_ops = None, None, None, None
        if mode != tf.estimator.ModeKeys.PREDICT:
            labels = features["label"]
            loss_op = tf.identity(tf.losses.sparse_softmax_cross_entropy(
                labels=labels, logits=logits),
                                  name="loss_op")
            train_op = tf.compat.v1.train.AdamOptimizer(
                learning_rate=0.001).minimize(
                    loss=loss_op,
                    global_step=tf.compat.v1.train.get_global_step())

            train_logging_hooks = [
                tf.estimator.LoggingTensorHook({"loss": "loss_op"},
                                               every_n_iter=20),
                tf.estimator.SummarySaverHook(
                    save_steps=save_summaries_every,
                    output_dir=summary_output_dir,
                    summary_op=tf.compat.v1.summary.merge_all())
            ]

            eval_metric_ops = {
                "accuracy": tf.metrics.accuracy(labels, predict_op)
            }

        # Squeeze prediction to scalar again
        if mode == tf.estimator.ModeKeys.PREDICT:
            predict_op = tf.squeeze(predict_op, name="output")

        return tf.estimator.EstimatorSpec(mode=mode,
                                          predictions=predict_op,
                                          loss=loss_op,
                                          train_op=train_op,
                                          training_hooks=train_logging_hooks,
                                          eval_metric_ops=eval_metric_ops)

    return model_fn


def main():
    global AUDIO_SHAPE

    parser = argparse.ArgumentParser()
    parser.add_argument("--mode",
                        required=True,
                        choices=[str(x).split(".")[1] for x in Mode],
                        help="one of (train_eval or export)")
    parser.add_argument(
        "--model_directory",
        required=True,
        help="Model directory -- where events & checkpoints is stored")
    parser.add_argument(
        "--goldfish_directory",
        help=
        "Base path of dataset. If none is provided GOLDFISH_DIRECTORY will be used"
    )
    parser.add_argument("--num_labels",
                        required=True,
                        type=int,
                        help="Number of labels in label_map")
    parser.add_argument("--clip_length",
                        default=1.0,
                        type=float,
                        help="Clip length in seconds of the audio")
    parser.add_argument("--sample_rate",
                        default=8000,
                        type=int,
                        help="Sample rate of the audio")
    parser.add_argument("--batch_size",
                        default=32,
                        type=int,
                        help="Batch size of training")
    parser.add_argument("--save_summary_every",
                        default=10000,
                        type=int,
                        help="Same summary every n steps")
    parser.add_argument("--eval_every",
                        default=10000,
                        type=int,
                        help="Evaluates on full dataset n steps")
    parser.add_argument("--max_steps",
                        default=10000000,
                        type=int,
                        help="Evaluates on full dataset n steps")
    parser.add_argument("--parallel_reads",
                        default=5,
                        type=int,
                        help="Parallel reads of dataset")

    args = parser.parse_args()

    AUDIO_SHAPE = int(args.clip_length * args.sample_rate)

    config = tf.estimator.RunConfig(
        model_dir=args.model_directory,
        save_summary_steps=args.save_summary_every,
        log_step_count_steps=100,
        save_checkpoints_steps=args.eval_every,
    )

    estimator = tf.estimator.Estimator(model_fn=make_model_fn(
        summary_output_dir=args.model_directory,
        num_labels=args.num_labels,
        sample_rate=args.sample_rate,
        save_summaries_every=args.save_summary_every),
                                       model_dir=args.model_directory,
                                       config=config)

    if args.mode == Mode.train_eval.value:
        train_spec = tf.estimator.TrainSpec(
            input_fn=create_input_fn(
                mode=tf.estimator.ModeKeys.TRAIN,
                goldfish_directory=args.goldfish_directory,
                parallel_reads=args.parallel_reads,
                batch_size=args.batch_size),
            max_steps=args.max_steps,
        )

        eval_spec = tf.estimator.EvalSpec(
            steps=args.eval_every,
            input_fn=create_input_fn(
                mode=tf.estimator.ModeKeys.EVAL,
                goldfish_directory=args.goldfish_directory,
                parallel_reads=args.parallel_reads,
                batch_size=args.batch_size),
            throttle_secs=30,
        )

        tf.estimator.train_and_evaluate(estimator, train_spec, eval_spec)

    elif args.mode == Mode.export.value:

        def serving_input_receiver_fn():
            inputs = {
                "audio":
                tf.compat.v1.placeholder(dtype=tf.int16,
                                         shape=[AUDIO_SHAPE],
                                         name="input")
            }
            return tf.estimator.export.ServingInputReceiver(
                features=inputs, receiver_tensors=inputs)

        estimator.export_saved_model(
            export_dir_base=args.model_directory,
            serving_input_receiver_fn=serving_input_receiver_fn)
    else:
        raise NotImplementedError(f"Unknown mode {args.mode}")


if __name__ == "__main__":
    main()
