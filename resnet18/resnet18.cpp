#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "bundle/resnet18.h"

using namespace std;

typedef struct
{
  char  *buf;
  unsigned long  size;
}__attribute__((packed))lne_data;

#define	BUFFER_SIZE	(1024*1024*12)
#define	OUT_BUF_SIZE	(1024)

uint16_t result_buf[OUT_BUF_SIZE];

int labels[1024];
float top_5[5];
int index_5[5];

// Load the weights & code into shared memory between ARM  & NMP processor
// This function is called twice (from main) for each model, one for
// weights and another for code.  
int loadBinary(int dev, const char* file_name, unsigned long address)
{
  int file_length;
  int ret;
  FILE *fd;
  char *buf;

  fd = fopen(file_name, "r");
  fseek(fd, 0, SEEK_END);
  file_length = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  buf = (char *) malloc(sizeof(char) * file_length);
  fread(buf, 1, file_length, fd);

  lseek(dev, address, SEEK_SET);
  ret = write(dev, buf, file_length);
  if (ret == -1)
    printf("writting failed\n");

  fclose(fd);
  return 0;
}

int loadImage(int dev, const char *file_name, unsigned long address) {

  /* The images have to be loaded in to a range of [0, 1] and then */
  /* normalized using mean = [0.485, 0.456, 0.406] */
  /* and stddev = [0.229, 0.224, 0.225]. */

  int ret;
  int size = 224 * 224 * 3 * 2;
  float scale = 255;
  float mean[3] = {0.485, 0.456, 0.406};
  float sdev[3] = {0.229, 0.224, 0.225};

  int16_t *ifm;
  ifm = (int16_t *)malloc(sizeof(int16_t) * size);

  cv::Mat input_image_bgr;
  cv::Mat input_image;

  input_image_bgr = cv::imread(file_name, cv::IMREAD_COLOR);
  cv::cvtColor(input_image_bgr, input_image, cv::COLOR_BGR2RGB);

  if (!input_image.data) {
    cout << "No image data" << endl;
    return -1;
  }

  if (input_image.cols != 224 || input_image.rows != 224) {
    cv::resize(input_image, input_image, cv::Size(224, 224), 0, 0,
               cv::INTER_LINEAR);
  }

  for (int c = 0; c < 3; c++) {
    for (int y = 0; y < input_image.rows; y++) {
      for (int x = 0; x < input_image.cols; x++) {
        uint8_t pix_value =
            input_image.data[(y * input_image.step + 3 * x + c)];
        float val = ((float)(pix_value/scale) - mean[c]) / sdev[c];
        ifm[(c * input_image.cols * input_image.rows) + (y * input_image.cols) +
            x] = (int16_t)(val * (1 << RESNET18_input_qpoint)) & 0xffff;
      }
    }
  }

  lseek(dev, address, SEEK_SET);
  ret = write(dev, ifm, size);
  if (ret == -1) printf("writting failed\n");
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
    index_5[i+1] = index_5[i];
    top_5[i+1] = top_5[i];
  }

  top_5[idx] = val;
  index_5[idx] = pos;
}

void ShowTops(int dev, unsigned long address, const char* file_name)
{
  int16_t result[1000];
  float val = 0.f;

  lseek(dev, address, SEEK_SET);
  int ret = read(dev, result, sizeof(result));

  if (ret == -1)
    printf("reading failed\n");

  for (int i = 0; i < 5; i++) {
    top_5[i] = -999.0;
    index_5[i] = -1;
  }

  for (int i = 0; i < 1000; i++) {
    val = (int16_t)result[i];
    val = (float) val;
    val = val / (1 << RESNET18_output_qpoint);

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

  int real_idx[5];
  for (int i = 0; i < 5; i++) {
    real_idx[i] = labels[index_5[i]];
  }

  printf("%s ", file_name);
  for (int i = 0; i < 5; i++) {
    printf("%d ", real_idx[i]);
  }
  printf("\n");

}

int main(int argc, char* argv[])
{
  std::string file_exec;
  if (argc == 2) {
    if (std::string(argv[1]).compare("acc") == 0) {
      file_exec = "../../datasets/imagenet/imagenet_accuracy.txt";
    } else if (std::string(argv[1]).compare("perf") == 0) {
      file_exec = "../../datasets/imagenet/imagenet_performance.txt";
    } else {
      std::cout << "Use acc | perf as argument\n";
      exit(1);
    }
  } else {
    std::cout << "Use acc | perf as argument\n";
    exit (1);
  }

  int dev, ret;
  char* fileName = argv[1];
  clock_t begin, end;
  lne_data param;
  double temp = 0;
  int i = 0;

  dev = open(LNE_DEVICE_NODE, O_RDWR|O_NDELAY);
  if(dev < 0){
    cerr << "[main] device file open error" << endl;
    return -1;
  }

  ifstream file("../../datasets/imagenet/synset_words.txt");

  string label;
  while (getline(file, label)) {
    labels[i] = atoi(label.c_str());
    i++;
  }

  loadBinary(dev, "./resnet18.weights.bin", (unsigned long)(LNE_DATA_ADDRESS));
  loadBinary(dev, "./resnet18.text.bin", (unsigned long)(LNE_TEXT_ADDRESS));

  memset(&param, 0, sizeof(lne_data));

  ifstream file_images(file_exec);
  string image;
  while(getline(file_images, image)) {
    loadImage(dev, image.c_str(), (unsigned long)(RESNET18_input_offset));
    memset(&param, 0, sizeof(lne_data));

    ret = ioctl(dev, LNE_IOCTL_CMD_RESET, &param);
    if(ret < 0)
    {
      cerr << "ioctl err :" << ret << endl;
    }

    ret = ioctl(dev, LNE_IOCTL_CMD_START, &param);
    if(ret < 0)
    {
      cerr << "ioctl err :" << ret << endl;
    }

    while(1) // Check whether running or not
    {
      if(!ioctl(dev, LNE_IOCTL_CMD_STOP, &param))
      {
        break;
      }
      usleep(10);
    }

    usleep(100000);
    ShowTops(dev, (unsigned long)(RESNET18_output_offset), image.c_str());
  }

  ret = close(dev);
  if(ret < 0)
  {
    cerr << "result close err :" << (int)ret << endl;
  }

  return 0;
}
