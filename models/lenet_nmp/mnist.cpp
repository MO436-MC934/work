#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "bundle/mnist.h"

using namespace std;

typedef struct {
    char *buf;
    unsigned long size;
} __attribute__((packed)) lne_data;

float top_5[6];
int index_5[6];
int dev;

int loadBinary(const char *file_name, unsigned long address) {
    int file_length;
    int ret;
    FILE *fd;
    char *buf;

    fd = fopen(file_name, "r");
    fseek(fd, 0, SEEK_END);
    file_length = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    buf = (char *)malloc(sizeof(char) * file_length);

    fread(buf, 1, file_length, fd);

    lseek(dev, address, SEEK_SET);
    ret = write(dev, buf, file_length);
    if (ret == -1)
        printf("writting failed\n");

    fclose(fd);
    return 0;
}

int loadImage(const char *file_name, unsigned long address) {
    int ret;
    float mean = 0;
    int size = 28 * 28 * 2;
    int bias = 0;
    float scale = 255;
    int16_t *ifm;
    ifm = (int16_t *)malloc(sizeof(int16_t) * size);

    cv::Mat input_image;
    input_image = cv::imread(file_name, cv::IMREAD_GRAYSCALE);
    if (!input_image.data) {
        cout << "No image data" << endl;
        return -1;
    }
    if (input_image.cols != 28 || input_image.rows != 28) {
        cv::resize(input_image, input_image, cv::Size(28, 28), 0, 0, CV_INTER_LINEAR);
    }
    for (int y = 0; y < input_image.rows; y++) {
        for (int x = 0; x < input_image.cols; x++) {
            uint8_t pix_value = input_image.data[(y * input_image.step + x)];
            float val = ((float)(pix_value - bias) - mean) / scale;
            ifm[(y * input_image.cols) + x] = (int16_t)(val * (1 << MNIST_input_qpoint)) & 0xffff;
        }
    }

    lseek(dev, address, SEEK_SET);
    ret = write(dev, ifm, size);
    if (ret == -1)
        printf("writting failed\n");
    free(ifm);

    return 0;
}

void swap_elements(int idx, int pos, float val) {
    int final_pos = 4;
    for (int i = 0; i < 5; i++) {
        if (top_5[i] == -999) {
            final_pos = i;
            i = 5;
        }
    }
    for (int i = final_pos; i >= idx; i--) {
        index_5[i + 1] = index_5[i];
        top_5[i + 1] = top_5[i];
    }
    top_5[idx] = val;
    index_5[idx] = pos;
}

void ShowTops(unsigned long address, const char *file_name) {
    int16_t result[10];
    float val = 0.f;

    lseek(dev, address, SEEK_SET);
    int ret = read(dev, result, sizeof(result));
    if (ret == -1)
        printf("reading failed\n");

    for (int i = 0; i < 5; i++) {
        top_5[i] = -999.0;
        index_5[i] = -1;
    }
    for (int i = 0; i < 10; i++) {
        val = (int16_t)result[i];
        val = (float)val;
        val = val / (1 << MNIST_output_qpoint);

        if (val > top_5[0]) {
            swap_elements(0, i, val);
        } else if (val > top_5[1]) {
            swap_elements(1, i, val);
        } else if (val > top_5[2]) {
            swap_elements(2, i, val);
        } else if (val > top_5[3]) {
            swap_elements(3, i, val);
        } else if (val > top_5[4]) {
            swap_elements(4, i, val);
        }
    }

    printf("%s ", file_name);
    for (int i = 0; i < 5; i++) {
        printf("%d ", index_5[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int ret;
    lne_data param;

    std::string file_exec;
    if (argc >= 2) {
        if (std::string(argv[1]).compare("acc") == 0) {
            file_exec = "../../datasets/mnist/mnist_accuracy.txt";
        } else if (std::string(argv[1]).compare("perf") == 0) {
            file_exec = "../../datasets/mnist/mnist_performance.txt";
        } else {
            std::cout << "Use acc | perf as argument\n";
            exit(1);
        }
    } else {
        std::cout << "Use acc | perf as argument\n";
        exit(1);
    }

    dev = open(LNE_DEVICE_NODE, O_RDWR | O_NDELAY);
    if (dev < 0) {
        cerr << "[main] device file open error" << endl;
        return -1;
    }

    loadBinary("./mnist.text.bin", (unsigned long)(LNE_TEXT_ADDRESS));
    loadBinary("./mnist.weights.bin", (unsigned long)(LNE_DATA_ADDRESS));

    ifstream file_images(file_exec);
    string image;
    while (getline(file_images, image)) {
        loadImage(image.c_str(), (unsigned long)(MNIST_input_offset));
        memset(&param, 0, sizeof(lne_data));
        ret = ioctl(dev, LNE_IOCTL_CMD_RESET, &param);
        if (ret < 0) {
            cerr << "ioctl err :" << ret << endl;
        }
        ret = ioctl(dev, LNE_IOCTL_CMD_START, &param);
        if (ret < 0) {
            cerr << "ioctl err :" << ret << endl;
        }
        while (1) { // Check whether running or not
            if (!ioctl(dev, LNE_IOCTL_CMD_STOP, &param)) {
                break;
            }
            usleep(10);
        }
        usleep(100000);
        ShowTops((unsigned long)(MNIST_output_offset), image.c_str());
    }

    ret = close(dev);
    if (ret < 0) {
        cerr << "result close err :" << (int)ret << endl;
    }
    return 0;
}
