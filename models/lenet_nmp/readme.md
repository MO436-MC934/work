# MNIST - Handwritten Digit Recognition

This model predicts handwritten digits using a convolutional neural network
(CNN). The model was downloaded from Onnx Model Zoo. This is the
[link](https://github.com/onnx/models/tree/master/vision/classification/mnist).

```
Model   ONNX version	Opset version	Top-1 accuracy (%)	Top-5 accuracy (%)
MNIST	      1.3	            8	            98.9	            -
```

## Inference on DQ1-A0

To compile the model to run the inference with NMP 2.4 you must use the
`nmp-backend` branch and set the environment variables:

```
export PATH=/path/to/riscv_nmp_toolchain/bin:/path/to/arm_toolchain/bin:$PATH
export OPENCV_PATH=/path/to/arm_toolchain/opencv
export RISCV_NMP_TOOLCHAIN=/path/to/riscv_nmp_toolchain
export NMPRTL_PATH=/path/to/libnmp
export GLOWBIN=/path/to/glow/build/bin
```

Measuring the accuracy on DQ1-A0 board:

```
NMP 2.4 accuracy
Top1: 98.2%
Top5: 99.8%
Precision: 98.1586%
Recall: 98.146%
F1-score: 98.1523%
```

## Inference on DQ1-B0

To compile the model to run the inference with NMP 2.5 you must use the
`nmp25-backend` branch and set the environment variables:

```
export PATH=/path/to/riscv_nmp25_toolchain/bin:/path/to/arm_toolchain/bin:$PATH
export OPENCV_PATH=/path/to/arm_toolchain/opencv
export RISCV_NMP_TOOLCHAIN=/path/to/riscv_nmp25_toolchain
export NMPRTL_PATH=/path/to/libnmp25/lib
export GLOWBIN=/path/to/glow/build/bin
```

Measuring the accuracy on DQ1-B0 board:

```
NMP 2.5 accuracy
Top1: 98.2%
Top5: 100%
Precision: 98.1586%
Recall: 98.146%
F1-score: 98.1523%
```
