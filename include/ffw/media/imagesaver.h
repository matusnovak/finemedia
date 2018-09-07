/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_IMAGE_SAVER
#define FFW_MEDIA_IMAGE_SAVER

#include <string>
#include "config.h"
#include "imagebuffer.h"
#include "swapwrapper.h"

#pragma warning( push )
#pragma warning( disable : 4275)

namespace ffw{
    /**
     * @ingroup media
     * @brief Base class for all image writers
     * @details This is an abstract class. Please use one of the specific image writers, such as
     * a PngWriter.
     */
    class FFW_API ImageWriter: public ImageFormat {
    public:
        ImageWriter();
        virtual ~ImageWriter();
        virtual void close() = 0;
        /**
         * @brief Writes a row of pixels
         * @param src Where to read pixels from
         * @details The src must contain an entire row of pixels. 
         * Use writer->getStrideSize() to determine the size of the row needed in number of bytes.
         */
        virtual size_t writeRow(const void* src) = 0;
        /**
         * @brief Writes the entire image from a single array
         * @param src The destination where to read the pixels from
         * @details The src parameter must be large enough to contain all pixels. Use 
         * writer->getStrideSize() multiplied by writer->getHeight() to get the number of bytes
         * needed for the source array.
         * @note This method also calls the writeFooter()
         */
        bool writeAll(const unsigned char* src);
        /**
         * @brief Writes the entire image from an image buffer
         * @note This method also calls the writeFooter()
         */
        bool writeAll(const ffw::ImageBuffer& buffer);
        virtual bool setMipMap(int level);
        /**
         * @brief Writes the footer
         * @note This is needed to be called at the end of writing all pixels. Not doing so could
         * lead to invalid image file.
         */
        virtual bool writeFooter() = 0;
        /**
         * @brief Returns true if we are at the end of the file
         */
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
         * @brief Returns the number of rows already written
         */
        inline int getRowOffset() const {
            return row;
        }
        /**
         * @brief Returns true if the file is open and ready for writing
         */
        inline bool isOpen() const {
            return loaded;
        }
        /**
         * @brief Returns the number of mipmaps the image contains
         * @details This is usually exactly one unless you are writing a DDS file.
         */
        inline int getNumOfMipMaps() const {
            return mipmaps;
        }
        /**
         * @brief Returns the number of mipmaps already written
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

