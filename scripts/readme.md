# Instructions for Inference

```
WORKDIR
  ├─ glow
  └─ work
      ├── datasets
      │   ├── imagenet
      │   ├── mnist
      ├── models
      │   ├── lenet
      │   ├── mnist
      │   ├── resnet18
      │   ├── mobilenet
      ├── scripts
      │   ├── exec_accuracy.sh
      │   ├── ground_truth_imagenet.txt
      │   ├── ground_truth_mnist.txt
      │   ├── measure_acc.cpp
      └── ...
```
The `exec_accuracy.sh` script performs the accuracy for the models. Just
following the example below in the scripts folder:

```
./exec_accuracy.sh -m resnet18
```

