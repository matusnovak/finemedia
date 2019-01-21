# FineMedia

|         | CircleCI | TravisCI | AppVeyor |
|---------|----------|----------|----------|
| master  | [![CircleCI](https://circleci.com/gh/matusnovak/finemedia/tree/master.svg?style=svg)](https://circleci.com/gh/matusnovak/finemedia/tree/master) | [![Build Status](https://travis-ci.org/matusnovak/finemedia.svg?branch=master)](https://travis-ci.org/matusnovak/finemedia) | [![Build status](https://ci.appveyor.com/api/projects/status/7ptts9y3grin6jqs/branch/master?svg=true)](https://ci.appveyor.com/project/matusnovak/finemedia/branch/devel) |
| devel  | [![CircleCI](https://circleci.com/gh/matusnovak/finemedia/tree/devel.svg?style=svg)](https://circleci.com/gh/matusnovak/finemedia/tree/devel) | [![Build Status](https://travis-ci.org/matusnovak/finemedia.svg?branch=devel)](https://travis-ci.org/matusnovak/finemedia) | [![Build status](https://ci.appveyor.com/api/projects/status/7ptts9y3grin6jqs/branch/devel?svg=true)](https://ci.appveyor.com/project/matusnovak/finemedia/branch/devel) |

*The following compilers are tested in the CI above: Visual Studio 2013, Visual Studio 2013 Win64, Visual Studio 2015, Visual Studio 2015 Win64, Visual Studio 2017, Visual Studio 2017 Win64, MinGW-w64 i686, MinGW-w64 x86_64, Linux GCC 5.5.0, Linux GCC 6.4.0, Linux GCC 5.5.0, Linux GCC 7.3.0, and OSX Clang 3.7*

## Introduction 

The FineMedia is a simple image reading and writing library, capable of writing and reading PNG, JPG, BMP, DDS (with mipmaps and cubemaps!), TIFF, PBM, and TGA files. The DDS format is capable of loading compressed textures (DTX and RGTC compression) usefull in OpenGL applications. Not only the DDS can load compressed textures, but also mipmaps and cubemaps. The library provides a simple `ffw::ImageBuffer` that has a very simple API to retrieve very detailed information about the pixel format. For example, you can get the number of bytes necessary to allocate n-th mipmap. Or, you can get detailed information of how many bits there is per one stride (one row of pixels in bytes). See the documentation for more. This library only gives you pixels, or writes pixels to an image file. This library does not process the pixels (scaling, roatating, effects) in any way. The following image formats and pixel formats are supported:

| Pixel Format        | BMP | DDS | JPG | PBM | PNG | TGA | TIFF |
| ------------------- | --- | --- | --- | --- | --- | --- | ---- |
| GRAYSCALE_4         | YES |     |     |     |     |     |      |
| GRAYSCALE_8         | YES | YES | YES | YES | YES | YES | YES  |
| GRAYSCALE_16        |     |     |     | YES | YES |     | YES  |
| GRAYSCALE_16F       |     | YES |     |     |     |     |      |
| GRAYSCALE_32F       |     | YES |     | YES |     |     | YES  |
| GRAYSCALE_ALPHA_8   |     |     |     |     | YES |     | YES  |
| GRAYSCALE_ALPHA_16  |     |     |     |     | YES |     | YES  |
| RG_1616             |     | YES |     |     |     |     |      |
| RG_1616F            |     | YES |     |     |     |     |      |
| RGB_444             | YES |     |     |     |     |     |      |
| RGB_888             | YES | YES | YES | YES | YES | YES | YES  |
| RGB_161616          | YES |     |     | YES | YES |     | YES  |
| RGB_161616F         | YES |     |     |     |     |     |      |
| RGB_323232F         | YES |     |     | YES |     |     | YES  |
| RGB_ALPHA_5551      |     |     |     |     |     | YES |      |
| RGB_ALPHA_4444      | YES | YES |     |     |     |     |      |
| RGB_ALPHA_8888      | YES | YES |     |     | YES | YES | YES  |
| RGB_ALPHA_16161616  | YES |     |     |     | YES |     | YES  |
| RGB_ALPHA_16161616F | YES | YES |     |     |     |     |      |
| RGB_ALPHA_32323232F | YES | YES |     |     |     |     | YES  |
| RGB_DXT1            |     | YES |     |     |     |     |      |
| RGB_ALPHA_DXT3      |     | YES |     |     |     |     |      |
| RGB_ALPHA_DXT5      |     | YES |     |     |     |     |      |
| RED_RGTC1           |     | YES |     |     |     |     |      |
| RED_GREEN_RGTC2     |     | YES |     |     |     |     |      |

And for the reference, here is a table of each pixel format with bits per channel.

_Please note that greyscale pixel formats only have one color (grey) and is represented by red channel.
The compressed textures do have multiple channels (RG, RGB, RGBA) but since they are compressed there
is no easy way to represent pixel depth per channel. Instead the red channel is used as a whole._

| Pixel Format        | Red | Green | Blue | Alpha | Is Float? | Is Compressed? |
| ------------------- | --- | --- | --- | --- | --- | --- |
| GRAYSCALE_4         | 4   |     |     | 0   |     |     |
| GRAYSCALE_8         | 8   |     |     | 0   |     |     |
| GRAYSCALE_16        | 16  |     |     | 0   |     |     |
| GRAYSCALE_16F       | 16  |     |     | 0   | YES |     |
| GRAYSCALE_32F       | 32  |     |     | 0   | YES |     |
| GRAYSCALE_ALPHA_8   | 8   |     |     | 8   |     |     |
| GRAYSCALE_ALPHA_16  | 16  |     |     | 16  |     |     |
| RG_1616             | 16  | 16  | 0   | 0   |     |     |
| RG_1616F            | 16  | 16  | 0   | 0   | YES |     |
| RGB_444             | 4   | 4   | 4   | 0   |     |     |
| RGB_888             | 8   | 8   | 8   | 0   |     |     |
| RGB_161616          | 16  | 16  | 16  | 0   |     |     |
| RGB_161616F         | 16  | 16  | 16  | 0   | YES |     |
| RGB_323232F         | 32  | 32  | 32  | 0   | YES |     |
| RGB_ALPHA_5551      | 5   | 5   | 5   | 1   |     |     |
| RGB_ALPHA_4444      | 4   | 4   | 4   | 4   |     |     |
| RGB_ALPHA_8888      | 8   | 8   | 8   | 8   |     |     |
| RGB_ALPHA_16161616  | 16  | 16  | 16  | 16  |     |     |
| RGB_ALPHA_16161616F | 16  | 16  | 16  | 16  | YES |     |
| RGB_ALPHA_32323232F | 32  | 32  | 32  | 32  | YES |     |
| RGB_DXT1            | 4   |     |     |     |     | YES |
| RGB_ALPHA_DXT3      | 8   |     |     |     |     | YES |
| RGB_ALPHA_DXT5      | 8   |     |     |     |     | YES |
| RED_RGTC1           | 8   |     |     |     |     | YES |
| RED_GREEN_RGTC2     | 8   |     |     |     |     | YES |


This library also bundles all third party libraries statically. Meaning, you only have to link the `.lib`  file (or `.so` on Linux). No need to manually download the libraries (GLFW, FreeType2, etc.), and spend an entire weekend figuring out how to plug it all together, and going through outdated or complicated instructions for some libraries. You either download the pre-built FineMedia from the GitHub releases, or build it manually (which is super easy), and you are good to go!

**API Documentation is provided at <http://matusnovak.github.io/finemedia/>**

## Dependencies

All dependencies listed here are already included as a git submodule and will be statically linked to the finemedia library. Therefore, no need to compile them manually, nor link them manually! Everything is automated via CMake.

* [libjpeg](https://github.com/LuaDist/libjpeg.git) - For reading and writing JPG images.
* [libtiff](https://gitlab.com/libtiff/libtiff.git) - For reading and writing TIFF images.
* [libpng](https://github.com/glennrp/libpng) - For reading and writing PNG images.
* [zlib](https://github.com/madler/zlib.git) - Needed by libpng.
* [bzip2](https://github.com/enthought/bzip2-1.0.6) - Needed by libtiff.

## TODO

* More image formats
* More pixel formats
* Better documentation with more examples

## Quick example

Create a rendering window and draw a rectangle:

```cpp
#include <ffw/media.h>
#include <iostream>

extern void doSomethingWithPixels(const char* pixels, int width, int height);

int main(int argc, char *argv[]) {
    const auto filename = "my_image.png";

    try {
        // Open image for reading
        const auto reader = std::make_unique<ffw::ImageReader>(
            ffw::openImageReader(filename)
        );

        // The library also contains:
        // void readImage(const std::string& path, ffw::ImageBuffer& image); - throws!
        // for quick and dirty way of reading images.

        // Print some stats
        std::cout << "> Image: " << filename << std::endl;
        std::cout << "size: " << reader->getWidth() << "x" << reader->getHeight() << std::endl;
        std::cout << "channels: " << reader->getNumberOfChannels() << std::endl;
        std::cout << "pixel bit depth: " << reader->getBitDepth() << std::endl;
        std::cout << "bits per pixel: " << reader->getBitsPerPixel() << std::endl;
        std::cout << "number of bytes per row: " << reader->getStrideSize() << std::endl;
        
        // The following image buffer will hold the pixels and information
        ffw::ImageBuffer image;

        // Read an entire image
        reader->readAll(image);

        // Optional (automatic on reader destruction)
        reader->close();

        // As an alternative, this works too:
        // std::unique_ptr<unsigned char[]> pixels;
        // pixels.reset(new unsigned char[reader->getAllocationSize()]);
        // reader->readAll(pixels.get());

        doSomethingWithPixels(image.getPtr(), image.getWidth(), image.getHeight());

        // We can then write the pixels to another image format
        const auto writer = std::make_unique<ffw::ImageWriter>(
            ffw::openImageWriter(
                "new_image.jpg",
                image.getWidth(),
                image.getHeight(),
                image.getType())
        );

        // Pass the ffw::ImageBuffer
        writer->writeAll(image);
        
        // Necessary!
        writer->writeFooter();

        // Optional (automatic on writer destruction)
        reader->close();
    } 
    catch (ffw::ImageException& e) {
        std::cerr 
            << "Something went wrong while loading " << filename 
            << " error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
```

## Requirements

One of the following compilers:

* (Windows) Visual Studio 2013 32-bit or 64-bit
* (Windows) Visual Studio 2015 32-bit or 64-bit
* (Windows) Visual Studio 2017 32-bit or 64-bit
* (Windows) MinGW-w64 i686
* (Windows) MinGW-w64 x86_64
* (Linux) GCC 4.9 or newer
* (OSX) Clang 3.7 or newer

## Compiling FineMedia

You can [use one of the automatically built releases from GitHub Releases](https://github.com/matusnovak/finemedia/releases) or compile it manually by following the [Install Guide](./INSTALL.md) (Don't worry, it's easy). 

## Using FineMedia

### Windows

First, compile the library (see section above), next to use the finemedia in your project, simply include the `C:/path/to/finemedia/include` and link the `finemedia.lib`. You will also need the `finemedia.dll` in order to run the application. Simply copy the DLL into the executable folder. You can find the `.lib` and `.dll` files in the cmake build folder.

### Linux/OSX

First, compile the library (see section above), next to use the finemedia in your project, simply include the `~/finemedia/include` (or `/usr/local/include` if you have installed it) and link the `finemedia.so`. You can find the `.so` files in the cmake build folder (or at the installed path provided by CMAKE_INSTALL_PREFIX).

If you are planning to distribute your application with FineMedia, you will need to copy the `finemedia.so` and include it within your application. For example, the most simple way, copy it into the executable folder (same as in Windows).

## Documentation

Documentation can be found at <http://matusnovak.github.io/finemedia/>

## Examples

All examples are located in the examples folder.

## Contributing

Feel free to post a question in GitHub issues. Pull requests are welcome! 

## Contact

Feel free to contact me on my email (see my GitHub profile).

## License

<https://en.wikipedia.org/wiki/MIT_License>

```
The MIT License (MIT)

Copyright (c) 2018 Matus Novak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```