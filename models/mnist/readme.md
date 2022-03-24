# MNIST - Handwritten Digit Recognition

This model predicts handwritten digits using a convolutional neural network
(CNN). The model was downloaded from Onnx Model Zoo, using the script below in
this [link](https://github.com/pytorch/glow/tree/master/utils).

```
./download_datasets_and_models.py -o lenet_mnist

```

After downloading the `mnist.onnx` model in the `/work/mnist` folder, you can
compile, build the application and run the inference following the steps:

``` 
make -f compile.mk
make -f builder.mk
cd bin
./main.x ../../../datasets/mnist/*.png
```

Measuring the accuracy on CPU:

```
Model	    ONNX version	Opset version	Top-1 accuracy (%)	Top-5 accuracy (%)
MNIST           1.3              8               98.9                -

Top1: 96.6667%
Top5: 100%
Precision: 97.5%
Recall: 96.6667%
F1-score: 97.0815%
```
