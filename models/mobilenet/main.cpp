/**
 * Copyright (c) Glow Contributors. See CONTRIBUTORS file.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <inttypes.h>
#include <libpng/png.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <string>
#include <vector>
#include <fstream>

#include "mobilenet.h"

/// This is an example demonstrating how to use auto-generated bundles and
/// create standalone executables that can perform neural network computations.
/// This example loads and runs the compiled mobilenet network model.
/// This example is using the static bundle API.

/// This example was modified by Dr. Marcio M Pereira to MO436 course

#define DEFAULT_HEIGHT 224
#define DEFAULT_WIDTH 224
#define OUTPUT_LEN 1000

float top_5[5];
int index_5[5];
int labels[1024];

//===----------------------------------------------------------------------===//
//                   Image processing helpers
//===----------------------------------------------------------------------===//
std::vector<std::string> inputImageFilenames;

/// \returns the index of the element at x,y,z,w.
size_t getXYZW(const size_t *dims, size_t x, size_t y, size_t z, size_t w) {
  return (x * dims[1] * dims[2] * dims[3]) + (y * dims[2] * dims[3]) +
         (z * dims[3]) + w;
}

/// \returns the index of the element at x,y,z.
size_t getXYZ(const size_t *dims, size_t x, size_t y, size_t z) {
  return (x * dims[1] * dims[2]) + (y * dims[2]) + z;
}

/// Reads a PNG image from a file into a newly allocated memory block \p imageT
/// representing a WxHxNxC tensor and returns it.
bool readPngImage(const char *filename, std::pair<float, float> range, float *&imageT, size_t *imageDims) {
    unsigned char header[8];
    // open file and test for it being a png.
    FILE *fp = fopen(filename, "rb");
    // Can't open the file.
    if (!fp)
        return true;

    // Validate signature.
    size_t fread_ret = fread(header, 1, 8, fp);
    if (fread_ret != 8)
        return true;
    if (png_sig_cmp(header, 0, 8))
        return true;

    // Initialize stuff.
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
        return true;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        return true;

    if (setjmp(png_jmpbuf(png_ptr)))
        return true;

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    size_t width = png_get_image_width(png_ptr, info_ptr);
    size_t height = png_get_image_height(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    const bool isGray = color_type == PNG_COLOR_TYPE_GRAY;
    const size_t numChannels = isGray ? 1 : 3;

    (void)bit_depth;
    assert(bit_depth == 8 && "Invalid image");
    assert((color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_RGB || isGray) && "Invalid image");
    bool hasAlpha = (color_type == PNG_COLOR_TYPE_RGB_ALPHA);

    int number_of_passes = png_set_interlace_handling(png_ptr);
    (void)number_of_passes;
    assert(number_of_passes == 1 && "Invalid image");

    png_read_update_info(png_ptr, info_ptr);

    // Error during image read.
    if (setjmp(png_jmpbuf(png_ptr)))
        return true;

    auto *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (size_t y = 0; y < height; y++) {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, info_ptr);

    imageDims[0] = width;
    imageDims[1] = height;
    imageDims[2] = numChannels;
    imageT = static_cast<float *>(calloc(1, width * height * numChannels * sizeof(float)));

    float scale = ((range.second - range.first) / 255.0);
    float bias = range.first;

    for (size_t row_n = 0; row_n < height; row_n++) {
        png_byte *row = row_pointers[row_n];
        for (size_t col_n = 0; col_n < width; col_n++) {
            png_byte *ptr = &(row[col_n * (hasAlpha ? (numChannels + 1) : numChannels)]);
            if (isGray) {
                imageT[getXYZ(imageDims, row_n, col_n, 0)] = float(ptr[0]) * scale + bias;
            } else {
                imageT[getXYZ(imageDims, row_n, col_n, 0)] = float(ptr[0]) * scale + bias;
                imageT[getXYZ(imageDims, row_n, col_n, 1)] = float(ptr[1]) * scale + bias;
                imageT[getXYZ(imageDims, row_n, col_n, 2)] = float(ptr[2]) * scale + bias;
            }
        }
    }

    for (size_t y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);
    return false;
}

/// Parse images file names into a vector.
void parseCommandLineOptions(int argc, char **argv) {
    int arg = 1;
    while (arg < argc) {
        inputImageFilenames.push_back(argv[arg++]);
    }
}

//===----------------------------------------------------------------------===//
//                 Wrapper code for executing a bundle
//===----------------------------------------------------------------------===//
/// Statically allocate memory for constant weights (model weights) and
/// initialize.
GLOW_MEM_ALIGN(MOBILENET_MEM_ALIGN)
uint8_t constantWeight[MOBILENET_CONSTANT_MEM_SIZE] = {
#include "mobilenet.weights.txt"
};

/// Statically allocate memory for mutable weights (model input/output data).
GLOW_MEM_ALIGN(MOBILENET_MEM_ALIGN)
uint8_t mutableWeight[MOBILENET_MUTABLE_MEM_SIZE];

/// Statically allocate memory for activations (model intermediate results).
GLOW_MEM_ALIGN(MOBILENET_MEM_ALIGN)
uint8_t activations[MOBILENET_ACTIVATIONS_MEM_SIZE];

/// Bundle input data absolute address.
uint8_t *inputAddr = GLOW_GET_ADDR(mutableWeight, MOBILENET_input);

/// Bundle output data absolute address.
uint8_t *outputAddr = GLOW_GET_ADDR(mutableWeight, MOBILENET_MobilenetV2_Predictions_Reshape_1__1);

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

/// Dump the result of the inference by looking at the results vector and
/// finding the top5 & the confidence of top1.
void ShowTops(const char *file_name) {
    float *results = (float *)(outputAddr);
    float val = 0.f;

    for (int i = 0; i < 5; i++) {
        top_5[i] = -999.0;
        index_5[i] = -1;
    }

    for (int i = 0; i < OUTPUT_LEN; i++) {
        val = results[i];
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

    // Original Tensorflow Mobilenet was trained with 1001 image classification
    int real_idx[5];
    for (int i = 0; i < 5; i++) {
        real_idx[i] = labels[index_5[i]-1];
    }

    printf("%s ", file_name);
    for (int i = 0; i < 5; i++) {
        printf("%d ", real_idx[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    parseCommandLineOptions(argc, argv);

    // load  labels
    int i = 0;
    std::ifstream file("../../../datasets/imagenet/synset_words.txt");
    std::string label;
    while (getline(file, label)) {
        labels[i] = atoi(label.c_str());
        i++;
    }

    unsigned numImages = inputImageFilenames.size();
    assert(numImages > 0 && "There must be at least one filename in filenames");
    // We iterate over all the png files, reading them into our input tensor
    // for processing
    size_t inputDims[4];
    inputDims[0] = 1;
    inputDims[1] = 3; // channels
    inputDims[2] = DEFAULT_HEIGHT;
    inputDims[3] = DEFAULT_WIDTH;
    size_t inputSizeInBytes = 3 * DEFAULT_HEIGHT * DEFAULT_WIDTH * sizeof(float);
    for (unsigned n = 0; n < numImages; n++) {
        float *inputT{nullptr};
        inputT = static_cast<float *>(malloc(inputSizeInBytes));
        float *imageT{nullptr};
        size_t dims[3];
        std::pair<float, float> range = std::make_pair(0., 1.0);
        bool loadSuccess = !readPngImage(inputImageFilenames[n].c_str(), range, imageT, dims);
        assert(loadSuccess && "Error reading input image.");
        assert((dims[0] == DEFAULT_HEIGHT && dims[1] == DEFAULT_WIDTH) &&
               "All images must have the same Height and Width");

        // Convert to BGR, as this is what NN is expecting.
        for (unsigned z = 0; z < 3; z++) {
            for (unsigned y = 0; y < dims[1]; y++) {
                for (unsigned x = 0; x < dims[0]; x++) {
                    inputT[getXYZW(inputDims, 0, 2 - z, x, y)] = imageT[getXYZ(dims, x, y, z)];
                }
            }
        }

        // Copy image data into the data input variable in the mutableWeightVars area.
        memcpy(inputAddr, inputT, inputSizeInBytes);

        // Perform the computation.
        int errCode = mobilenet(constantWeight, mutableWeight, activations);
        if (errCode != GLOW_SUCCESS) {
            printf("Error running bundle: error code %d\n", errCode);
        }

        // Print results.
        ShowTops(inputImageFilenames[n].c_str());
    }
}
