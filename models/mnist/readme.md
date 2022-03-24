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

## Measuring the accuracy on CPU:

To measure the accuracy of `mnist`, go to the `/work/scripts` folder and run the script:

``` 
./exec_accuracy.sh -m mnist

```

Using the CNTK as the framework to perform the inference in the test set (10.000 images)
of the popular [MNIST dataset](http://yann.lecun.com/exdb/mnist/), the TOP-1 error
rate is equal 1.1%.

```
Model	    ONNX version	Opset version	Top-1 accuracy (%)	Top-5 accuracy (%)
MNIST           1.3              8                98.9                     -
```

In my CPU, with only 30 images (that explains the Top-1 error rate of 3.33%):

```
Top1: 96.6667%
Top5: 100%
Precision: 97.5%
Recall: 96.6667%
F1-score: 97.0815%
```
