#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

using namespace std;

int convertImage(const char *file_in, const char* file_out, int size) {
  cv::Mat input_image_bgr;
  cv::Mat input_image;

  input_image_bgr = cv::imread(file_in, cv::IMREAD_COLOR);
  cv::cvtColor(input_image_bgr, input_image, cv::COLOR_BGR2RGB);

  if (!input_image.data) {
    cout << "No image data" << endl;
    return -1;
  }

  if (input_image.cols != size || input_image.rows != size) {
    cv::resize(input_image, input_image, cv::Size(size, size), 0, 0,
               cv::INTER_LINEAR);
  }
  cv::imwrite(file_out, input_image);
  return 0;
}

int main(int argc, char* argv[])
{
  std::string file_exec;
  char* filein = argv[1];
  char* fileout = argv[2];
  int size = atoi(argv[3]);
  convertImage(filein, fileout, size);
  return 0;
}
