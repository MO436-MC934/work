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
