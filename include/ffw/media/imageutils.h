/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_IMAGE_UTILS
#define FFW_MEDIA_IMAGE_UTILS

#include <string>
#include "config.h"
#include "imagebuffer.h"

namespace ffw {
    class ImageReader;
    class ImageWriter;
    /**
    * @ingroup media
    */
    FFW_API ImageReader* openImageReader(const std::string& path);
    /**
    * @ingroup media
    */
    FFW_API ImageWriter* openImageWriter(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);
    /**
    * @ingroup media
    */
    void FFW_API readImage(const std::string& path, void** dest, int* width, int* height, ffw::ImageType* format, int* mips = NULL);
    /**
    * @ingroup media
    */
    void FFW_API writeImage(const std::string& path, const void* src, int width, int height, ffw::ImageType format, int quality = 100, int mips = 1);
    /**
    * @ingroup media
    */
    void FFW_API readImage(const std::string& path, ffw::ImageBuffer& image);
    /**
    * @ingroup media
    */
    void FFW_API writeImage(const std::string& path, const ffw::ImageBuffer& image, int quality = 100);
}

#endif