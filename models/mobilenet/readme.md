# Mobilenet v2

This model was extracted from Tensorflow Hosted Models and converted to Onnx
model. The version used was `Mobilenet_V2_1.0_224` extracted in this
[link](https://www.tensorflow.org/lite/guide/hosted_models#floating_point_models).

To convert from tensorflow to onnx model we need to install the `tflite2onnx`
application (see: https://github.com/jackwish/tflite2onnx) and use the command
line below:

```
tflite2onnx mobilenet_v2_1.0_224.tflite mobilenet.onnx
```

## Measuring the accuracy on CPU:

```
Model name	Model size	Top-1 accuracy	Top-5 accuracy
mobilenet	13.0 Mb	       71.8%	        90.6%

CPU accuracy
Top1: 70.1%
Top5: 90.5%
Precision: 69.8499%
Recall: 70.6367%
F1-score: 70.2411%
```
