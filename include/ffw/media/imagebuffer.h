/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_IMAGE_BUFFER
#define FFW_MEDIA_IMAGE_BUFFER

#include "config.h"
#include <cstring>
#include <algorithm>
#include <memory>
#include <cmath>

namespace ffw {
    /**
    * @ingroup media
    */
    enum class ImageType {
        INVALID = 0,
        BITMAP_1,
        GRAYSCALE_4,
        GRAYSCALE_ALPHA_4,
        GRAYSCALE_8,
        GRAYSCALE_ALPHA_8,
        GRAYSCALE_16,
        GRAYSCALE_16F,
        GRAYSCALE_ALPHA_16,
        GRAYSCALE_ALPHA_16F,
        GRAYSCALE_32F,
        GRAYSCALE_ALPHA_32F,
        RG_88,
        RG_1616,
        RG_1616F,
        RG_3232F,
        RGB_565,
        RGB_888,
        RGB_161616,
        RGB_161616F,
        RGB_323232F,
        RGB_ALPHA_5551,
        RGB_ALPHA_4444,
        RGB_ALPHA_8888,
        RGB_ALPHA_16161616,
        RGB_ALPHA_16161616F,
        RGB_ALPHA_32323232F,
        RGB_DXT1,
        RGB_ALPHA_DXT3,
        RGB_ALPHA_DXT5,
        RED_RGTC1,
        RED_GREEN_RGTC2,
        SIGNED_RED_RGTC1,
        SIGNED_RED_GREEN_RGTC2,
        CUSTOM,
    };
    /**
    * @ingroup media
    */
    class ImageFormat {
    public:
        inline ImageFormat():
            width(0),height(0),depth(0),format(ffw::ImageType::INVALID){
        }
        inline ImageFormat(int width, int height, int depth, ffw::ImageType format):
            width(width),height(height),depth(depth),format(format){
        }
        inline ImageFormat(const ImageFormat& other):
            width(other.width),height(other.height),depth(other.depth),format(other.format){
        }
        /**
         * @brief Returns the image type enum of this format
         */
        inline ImageType getImageType() const {
            return format;
        }
        /**
         * @brief Returns the width
         * @param mipmap Optional mipmap level (default is zero)
         */
        inline int getWidth(int mipmap = 0) const {
            if (mipmap <= 0)return width;
            return width >> mipmap;
        }
        /**
         * @brief Returns the height
         * @param mipmap Optional mipmap level (default is zero)
         */
        inline int getHeight(int mipmap = 0) const {
            if (mipmap <= 0)return height;
            return height >> mipmap;
        }
        /**
         * @brief Returns the depth
         * @param mipmap Optional mipmap level (default is zero)
         */
        inline int getDepth(int mipmap = 0) const {
            if (mipmap <= 0)return depth;
            return depth >> mipmap;
        }
        /*
         * @brief Returns the number of bytes needed for one single row of pixels
         * @param mipmap Optional mipmap level (default is zero)
         */
        inline size_t getStrideSize(int mipmap = 0) const {
            if(isCompressed()) {
                return getMipMapSize(mipmap) / getHeight(mipmap);
            }
            return size_t(std::ceil(getWidth(mipmap) * (getBitsPerPixel() / 8.0f)));
        }
        /**
         * @brief Returns the number of bits per one pixels
         * @details For example a format of RGB_888 has 24 bits per pixel
         */
        inline int getBitsPerPixel() const {
            switch (format) {
                case ImageType::BITMAP_1: return 1;
                case ImageType::GRAYSCALE_4: return 4;
                case ImageType::GRAYSCALE_ALPHA_4: return 8;
                case ImageType::GRAYSCALE_8: return 8;
                case ImageType::GRAYSCALE_ALPHA_8: return 16;
                case ImageType::GRAYSCALE_16: return 16;
                case ImageType::GRAYSCALE_16F: return 16;
                case ImageType::GRAYSCALE_ALPHA_16: return 32;
                case ImageType::GRAYSCALE_ALPHA_16F: return 32;
                case ImageType::GRAYSCALE_32F: return 32;
                case ImageType::GRAYSCALE_ALPHA_32F: return 64;
                case ImageType::RGB_565: return 16;
                case ImageType::RGB_888: return 24;
                case ImageType::RGB_161616: return 48;
                case ImageType::RGB_323232F: return 96;
                case ImageType::RGB_ALPHA_5551: return 16;
                case ImageType::RGB_ALPHA_4444: return 16;
                case ImageType::RGB_ALPHA_8888: return 32;
                case ImageType::RGB_ALPHA_16161616: return 64;
                case ImageType::RGB_ALPHA_16161616F: return 64;
                case ImageType::RGB_ALPHA_32323232F: return 128;
                case ImageType::RGB_DXT1: return 4;
                case ImageType::RGB_ALPHA_DXT3: return 8;
                case ImageType::RGB_ALPHA_DXT5: return 8;
                case ImageType::RED_RGTC1: return 4;
                case ImageType::SIGNED_RED_RGTC1: return 4;
                case ImageType::RED_GREEN_RGTC2: return 8;
                case ImageType::SIGNED_RED_GREEN_RGTC2: return 8;
                case ImageType::RG_88: return 16;
                case ImageType::RG_1616: return 32;
                case ImageType::RG_1616F: return 32;
                case ImageType::RG_3232F: return 48;
                default: return 0;
            }
        }
        /**
         * @brief Returns the bit depth
         * @details For example, a format of RGB_ALPHA_16161616F has bit depth of 16
         */
        inline int getBitDepth() const {
            switch (format) {
                case ImageType::BITMAP_1: return 1;
                case ImageType::GRAYSCALE_4: return 4;
                case ImageType::GRAYSCALE_ALPHA_4: return 4;
                case ImageType::GRAYSCALE_8: return 8;
                case ImageType::GRAYSCALE_ALPHA_8: return 8;
                case ImageType::GRAYSCALE_16: return 16;
                case ImageType::GRAYSCALE_16F: return 16;
                case ImageType::GRAYSCALE_ALPHA_16: return 16;
                case ImageType::GRAYSCALE_ALPHA_16F: return 16;
                case ImageType::GRAYSCALE_32F: return 32;
                case ImageType::GRAYSCALE_ALPHA_32F: return 32;
                case ImageType::RGB_565: return 5;
                case ImageType::RGB_888: return 8;
                case ImageType::RGB_161616: return 16;
                case ImageType::RGB_323232F: return 32;
                case ImageType::RGB_ALPHA_5551: return 5;
                case ImageType::RGB_ALPHA_4444: return 4;
                case ImageType::RGB_ALPHA_8888: return 8;
                case ImageType::RGB_ALPHA_16161616: return 16;
                case ImageType::RGB_ALPHA_16161616F: return 16;
                case ImageType::RGB_ALPHA_32323232F: return 32;
                case ImageType::RGB_DXT1: return 8;
                case ImageType::RGB_ALPHA_DXT3: return 8;
                case ImageType::RGB_ALPHA_DXT5: return 8;
                case ImageType::RED_RGTC1: return 8;
                case ImageType::SIGNED_RED_RGTC1: return 8;
                case ImageType::RED_GREEN_RGTC2: return 8;
                case ImageType::SIGNED_RED_GREEN_RGTC2: return 8;
                case ImageType::RG_88: return 8;
                case ImageType::RG_1616: return 16;
                case ImageType::RG_1616F: return 16;
                case ImageType::RG_3232F: return 32;
                default: return 0;
            }
        }
        /**
         * @brief Returns the number of channels
         * @details For example, a format of RGB_ALPHA_8888 has 4 channels.
         */
        inline int getNumberOfChannels() const {
            switch (format) {
                case ImageType::BITMAP_1: return 1;
                case ImageType::GRAYSCALE_4: return 1;
                case ImageType::GRAYSCALE_ALPHA_4: return 2;
                case ImageType::GRAYSCALE_8: return 1;
                case ImageType::GRAYSCALE_ALPHA_8: return 2;
                case ImageType::GRAYSCALE_16: return 1;
                case ImageType::GRAYSCALE_16F: return 1;
                case ImageType::GRAYSCALE_ALPHA_16: return 2;
                case ImageType::GRAYSCALE_ALPHA_16F: return 2;
                case ImageType::GRAYSCALE_32F: return 1;
                case ImageType::GRAYSCALE_ALPHA_32F: return 2;
                case ImageType::RGB_565: return 3;
                case ImageType::RGB_888: return 3;
                case ImageType::RGB_161616: return 3;
                case ImageType::RGB_323232F: return 3;
                case ImageType::RGB_ALPHA_5551: return 4;
                case ImageType::RGB_ALPHA_4444: return 4;
                case ImageType::RGB_ALPHA_8888: return 4;
                case ImageType::RGB_ALPHA_16161616: return 4;
                case ImageType::RGB_ALPHA_16161616F: return 4;
                case ImageType::RGB_ALPHA_32323232F: return 4;
                case ImageType::RGB_DXT1: return 3;
                case ImageType::RGB_ALPHA_DXT3: return 4;
                case ImageType::RGB_ALPHA_DXT5: return 4;
                case ImageType::RED_RGTC1: return 1;
                case ImageType::SIGNED_RED_RGTC1: return 1;
                case ImageType::RED_GREEN_RGTC2: return 2;
                case ImageType::SIGNED_RED_GREEN_RGTC2: return 2;
                case ImageType::RG_88: return 2;
                case ImageType::RG_1616: return 2;
                case ImageType::RG_1616F: return 2;
                case ImageType::RG_3232F: return 2;
                default: return 0;
            }
        }
        /**
         * @brief Returns true if the pixels are compressed
         * @details Only accepts RGB_DXT1, RGB_ALPHA_DXT3, RGB_ALPHA_DXT5, RED_RGTC1, 
         * SIGNED_RED_RGTC1, RED_GREEN_RGTC2, and SIGNED_RED_GREEN_RGTC2 as valid compressed
         * pixels.
         */
        inline bool isCompressed() const {
            switch (format) {
                case ImageType::RGB_DXT1:
                case ImageType::RGB_ALPHA_DXT3:
                case ImageType::RGB_ALPHA_DXT5: 
                case ImageType::RED_RGTC1:
                case ImageType::SIGNED_RED_RGTC1:
                case ImageType::RED_GREEN_RGTC2:
                case ImageType::SIGNED_RED_GREEN_RGTC2:
                    return true;
                default: return false;
            }
        }
        /**
         * @brief Returns the size of a mipmap (in bytes) given a mipmap level
         * @param level Must be zero or higher
         */
        inline size_t getMipMapSize(int level) const {
            int w = getWidth(level) == 0 ? 1 : getWidth(level);
            int h = getHeight(level) == 0 ? 1 : getHeight(level);
            int d = getDepth(level) == 0 ? 1 : getDepth(level);
            switch(format) {
                case ImageType::RED_RGTC1:
                case ImageType::SIGNED_RED_RGTC1:
                case ImageType::RGB_DXT1:
                    return ((w+3)/4) * ((h+3)/4) * d * 8;
                case ImageType::RED_GREEN_RGTC2:
                case ImageType::SIGNED_RED_GREEN_RGTC2:
                case ImageType::RGB_ALPHA_DXT3:
                case ImageType::RGB_ALPHA_DXT5:
                    return ((w+3)/4) * ((h+3)/4) * d * 16;
                default:
                    return getStrideSize(level) * h * d;
            }
        }
        /**
         * @brief Returns the pointer to the beginning of a mipmap pixels
         * @param level Must be zero or higher
         */
        inline size_t getMipMapPtrOffset(int level) const {
            size_t cpy = 0;
            for (int i = 0; i < level; i++) {
                cpy += getMipMapSize(i);
            }
            return cpy;
        }
        inline void swap(ImageFormat& other) {
            using std::swap;
            swap(width, other.width);
            swap(height, other.height);
            swap(depth, other.depth);
            swap(format, other.format);
        }
        inline ImageFormat& operator = (const ImageFormat& other) {
            width = other.width;
            height = other.height;
            depth = other.depth;
            format = other.format;
            return *this;
        }
        /**
         * @brief Returns the number of bytes needed to allocate an image provided number of mipmaps
         */
        inline size_t getAllocationSize(int mipmaps) const {
            size_t size = 0;
            for (int i = 0; i < mipmaps; i++) {
                size += getMipMapSize(i);
            }
            return size;
        }
        inline bool operator==(const ImageFormat& other) const {
            return width == other.width &&
                height == other.height &&
                depth == other.depth &&
                format == other.format;
        }
    protected:
        ffw::ImageType format;
        int width;
        int height;
        int depth;
    };
    /**
    * @ingroup media
    */
    class ImageBuffer: public ImageFormat {
    public:
        inline ImageBuffer() {
            mipmaps = 0;
        }
        ImageBuffer(const ImageBuffer& other) = delete;
        inline ImageBuffer(ImageBuffer&& other):ptr(std::move(other.ptr)) {
            other.ptr.reset();
            width = other.width;
            height = other.height;
            depth = other.depth;
            format = other.format;
            mipmaps = other.mipmaps;
        }
        inline virtual ~ImageBuffer() {
            destroy();
        }
        inline bool isAllocated() const {
            return ptr != NULL;
        }
        inline operator bool() const {
            return isAllocated();
        }
        inline int getNumOfMipMaps() const {
            return mipmaps;
        }
        inline void swap(ImageBuffer& second) {
            ImageFormat::swap(second);
            std::swap(ptr, second.ptr);
            std::swap(mipmaps, second.mipmaps);
        }
        inline void destroy() {
            if (ptr != NULL) {
                ptr.reset();
            }
            format = ffw::ImageType::INVALID;
            width = 0;
            depth = 0;
            height = 0;
            mipmaps = 0;
        }
        inline bool allocate(int w, int h, ffw::ImageType type, const void* pixels = NULL, int mips = 0) {
            if (w <= 0 || h <= 0)return false;
            if (mips < 0)return false;
            if (type == ffw::ImageType::INVALID)return false;

            destroy();
            width = w;
            height = h;
            depth = 0;
            format = type;
            mipmaps = mips;
            ptr.reset(new unsigned char[getAllocationSize(mipmaps)]);
            if (pixels != NULL)memcpy(ptr.get(), pixels, getAllocationSize(mipmaps));
            return true;
        }
        inline unsigned char* getPtr() {
            return ptr.get();
        }
        inline const unsigned char* getPtr() const {
            return ptr.get();
        }
        inline unsigned char* getMipMapPtr(int level) {
            return ptr.get() + getMipMapPtrOffset(level);
        }
        inline const unsigned char* getMipMapPtr(int level) const {
            return ptr.get() + getMipMapPtrOffset(level);
        }
        inline size_t getSize() const {
            return height * getStrideSize();
        }
        ImageBuffer& operator = (const ImageBuffer& other) = delete;
        inline ImageBuffer& operator = (ImageBuffer&& other) {
            if (this != &other) {
                swap(other);
            }
            return *this;
        }
        inline bool operator==(const ImageBuffer& other) const {
            return ImageFormat::operator==(other) && mipmaps == other.mipmaps && ptr == other.ptr;
        }
    private:
        std::unique_ptr<unsigned char> ptr;
        int mipmaps;
    };
};

inline void swap(ffw::ImageBuffer& first, ffw::ImageBuffer& second) {
    first.swap(second);
}

#endif