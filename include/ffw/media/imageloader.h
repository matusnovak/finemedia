/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_IMAGE_LOADER
#define FFW_MEDIA_IMAGE_LOADER

#include <string>
#include "config.h"
#include "imagebuffer.h"
#include "swapwrapper.h"

#pragma warning( push )
#pragma warning( disable : 4275)

namespace ffw{
    /**
     * @ingroup media
     */
    class FFW_API ImageReader: public ImageFormat {
    public:
        ImageReader();
        virtual ~ImageReader();
        /**
         * @brief Closes the file and frees the resources
         */
        virtual void close() = 0;
        /**
         * @brief Reads a row of pixels
         * @param dest The destination where to write the pixels
         * @note The dest parameter must be large enough to contain all pixels of a single row. 
         * Use reader->getStrideSize() to determine the size of the row in number of bytes.
         * @see 
         */
        virtual size_t readRow(void* dest) = 0;
        virtual bool getMipMap(int level);
        /**
         * @brief Reads the entire image into a single array
         * @param dest The destination where to write the pixels
         * @details The dest parameter must be large enough to contain all pixels. Use 
         * reader->getStrideSize() multiplied by reader->getHeight() to get the number of bytes
         * needed for the destination array.
         */
        bool readAll(void* dest);
        /**
         * @brief Reads the entire image into an image buffer
         */
        bool readAll(ffw::ImageBuffer& image);
        inline bool eof() const {
            return row >= height;
        }
        /**
         * @brief Same as isOpen()
         */
        inline operator bool() const {
            return loaded;
        }
        /**
         * @brief Returns the number of rows already read
         */
        inline int getRowOffset() const {
            return row;
        }
        /**
         * @brief Returns true if the image file is open and ready for reading
         */
        inline bool isOpen() const {
            return loaded;
        }
        /**
         * @brief Returns the number of mipmaps the image contains
         * @details This is usually exactly one unless you are reading a DDS file.
         */
        inline int getNumOfMipMaps() const {
            return mipmaps;
        }
        /**
         * @brief Returns the number of mipmaps already read
         */
        inline int getMipMapOffset() const {
            return mipmapOffset;
        }
        /**
         * @brief Returns the number of bytes needed to allocate an array of pixels
         */
        inline size_t getAllocationSize() const {
            return ImageFormat::getAllocationSize(mipmaps);
        }
    protected:
        bool loaded;
        int row;
        int mipmapOffset;
        int mipmaps;
    };
};

#pragma warning( pop ) 
#endif

