== Image Diff ==

Provides a metric for measuring the difference between two PNG images.

Prints out a single floating-point number between 0 and 1.

  * 0 means that the images are identical.
  * 1 means that each pixel is maximally different in each channel.

A non-zero return value indicates that something went wrong.

This program is intended for use in scripts to sort pairs of images by
sameness.

== Build ==

On Posix systems with libpng 1.6+ installed in standard locations:

    git clone https://github.com/HalCanary/imagediff.git
    cd imagediff
    make
    ./imgdiff FILE1.png FILE2.png

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
