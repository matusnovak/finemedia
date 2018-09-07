/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_EXCEPTIONS
#define FFW_MEDIA_EXCEPTIONS

#include <string>

namespace ffw {
    /**
    * @ingroup graphics
    */
    class ImageException : public std::runtime_error {
    public:
        ImageException(const char* msg) :std::runtime_error(msg) {

        }
        ImageException(const std::string& msg) :std::runtime_error(msg) {

        }

        static ImageException invalidFile() {
            return ImageException("Failed to open file");
        }

        static ImageException invalidHeader() {
            return ImageException("Image contains invalid header");
        }

        static ImageException invalidFormat() {
            return ImageException("Image contains unsupported pixel format");
        }

        static ImageException invalidFileSize() {
            return ImageException("File is too small to contain entire image data");
        }

        static ImageException invalidWidthHeight() {
            return ImageException("width or height is invalid");
        }

        static ImageException unsupportedImageType() {
            return ImageException("This image type does not support the requested image format");
        }
    };
};

#endif
