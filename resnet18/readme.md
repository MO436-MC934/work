# ONNX ResNet18-v1

You need to download the model from Onnx Model Zoo. This is the
[link](https://github.com/onnx/models/tree/master/vision/classification/resnet).

The image below shows the version we are working on.
![resnet18](../images/resnet18.png)

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
Model	    ONNX version	Opset version	Top-1 accuracy (%)	Top-5 accuracy (%)
ResNet18	1.2.1	        7	            69.93	            89.29

NMP 2.4 accuracy
Top1: 66.8%
Top5: 87.2%
Precision: 65.6677%
Recall: 66.8375%
F1-score: 66.2474%
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
Model	    ONNX version	Opset version	Top-1 accuracy (%)	Top-5 accuracy (%)
ResNet18	1.2.1	        7	            69.93	            89.29

NMP 2.5 accuracy
Top1: 67.4%
Top5: 88%
Precision: 66.2422%
Recall: 67.6812%
F1-score: 66.954%
```
