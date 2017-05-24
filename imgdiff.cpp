/*
  Copyright 2017 Google Inc.

  Use of this source code is governed by a BSD-style license that can be
  found in the LICENSE file.

  == Image Diff ==

  Provides a metric for measuring the difference between two PNG images.
  Prints out a single floating-point number between 0.0 and 1.0.
    0.0 means that the images are identical.
    1.0 means that each pixel is maximally different in each channel.
  A non-zero return value indicates that something went wrong.

  This program is intended for use in scripts to sort pairs of images by
  sameness.

  On MacOS with Homebrew:
    brew install libpng
    git clone https://github.com/HalCanary/imagediff.git
    cd imagediff
    ./maker.sh "$(brew --prefix)"
    ./imgdiff FILE1.png FILE2.png

  On Posix system without libpng1.6+ already installed:
    # Download source to libpng version 1.6+...
    md5sum ~/Downloads/libpng-1.6.29.tar.xz
    # verify checksum
    cd /tmp
    tar xf ~/Downloads/libpng-1.6.29.tar.xz
    cd libpng-1.6.29
    PREFIX=~/local
    ./configure --prefix="$PREFIX" && make && make install
    cd /tmp
    git clone https://github.com/HalCanary/imagediff.git
    cd imagediff
    ./maker.sh "$PREFIX"
    ./imgdiff FILE1.png FILE2.png
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
        fprintf(stderr, "png error [%s]: %s\n", path, image.message);
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
        fprintf(stderr, "png errori [%s]: %s\n", path, image.message);
        exit(4);
    }
    return std::move(output);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        const char usage[] = "\nUsage:\n  %s {FILE1}.png {FILE2}.png\n\n";
        fprintf(stderr, usage, argv[0]);
        return 1;
    }
    Img img1 = read_png(argv[1]);
    Img img2 = read_png(argv[2]);
    if (img1.width != img2.width || img1.height != img2.height) {
        fprintf(stderr, "\nImages must be same size: (%d,%d) != (%d,%d)\n\n",
                img1.width, img1.height, img2.width, img2.height);
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

