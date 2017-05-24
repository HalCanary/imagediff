/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// Provides a metric for measureing the difference between two images.
// Prints out a single floating-point number between 0.0 and 1.0.
// 0.0 means that the images are identical.
// 1.0 means that each pixel is maximally different in each channel.
// A non-zero return value indicates that something went wrong.
//
// This program is intentionally non-verbose and non-helpful, as it is intended
// for use in scripts to sort pairs of images by sameness.

/*
  On MacOS:
    brew install libpng
    BP="$(brep --prefix)"
    CPPFLAGS="-I \"$BP/include\"" LDFLAGS="-L \"$BP/lib\"" make
*/

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

extern "C" {
#include <png.h>
}

struct Img {
    std::unique_ptr<unsigned char[]> data;
    int width;
    int height;
};

Img read_png(const char* path) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    if (0 == png_image_begin_read_from_file(&image, path)) {
        fprintf(stderr, "png error: %s\n", image.message);
        exit(3);
    }
    image.format = PNG_FORMAT_RGBA;
    assert(image.width > 0);
    assert(image.height > 0);
    assert(PNG_IMAGE_SIZE(image) == 4 * image.width * image.height);
    Img output;
    output.data.reset(new unsigned char[PNG_IMAGE_SIZE(image)]);
    output.width = image.width;
    output.height = image.height;
    if (0 == png_image_finish_read(&image, nullptr, output.data.get(), 0, nullptr)) {
        png_image_free(&image);
        fprintf(stderr, "png error: %s\n", image.message);
        exit(4);
    }
    return std::move(output);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        return 1;
    }
    Img img1 = read_png(argv[1]);
    Img img2 = read_png(argv[2]);
    if (img1.width != img2.width || img1.height != img2.height) {
        return 2;
    }
    const size_t N = 4 * (size_t)img1.width * (size_t)img1.height;
    int64_t totalDiffs = 0; // Manhattan distance in ARGB color-space.
    for (size_t i = 0; i < N; ++i) {
        totalDiffs += abs((int)img1.data[i] - (int)img2.data[i]);
    }
    printf("%g\n", (double)totalDiffs / (255 * N));
    return 0;
}

