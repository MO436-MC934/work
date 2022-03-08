# Squeezenet

This model was extracted from Tensorflow Hosted Models and converted to Onnx
model. The mopdel used was  extracted in this
[link](https://www.tensorflow.org/lite/guide/hosted_models#floating_point_models).

To convert from tensorflow to onnx model we need to install the `tflite2onnx`
application (see: https://github.com/jackwish/tflite2onnx) and use the command
line below:

```
tflite2onnx squeezenet.tflite squeezenet.onnx
```

Measuring the accuracy on CPU:

```
Model name	Model size	Top-1 accuracy	Top-5 accuracy
SqueezeNet	5.0 Mb	       49.0%	        72.9%

CPU accuracy
Top1: 48.8%
Top5: 71.6%
Precision: 48.5197%
Recall: 49.0192%
F1-score: 48.7681%
```
## Note on pre-processing images for Squeezenet

Despite the
[site](https://tfhub.dev/tensorflow/lite-model/squeezenet/1/default/1) saying
that it was trained with pre-processed images with a mean and standard
deviation of 127.5, the inference does not perform well. I changed the standard
deviation to 255 and kept the average at 127.5. Preprocessing the images with
these new parameters, the expected value for top1 and top5 was reached.
