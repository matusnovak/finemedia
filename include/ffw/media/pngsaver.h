/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_PNG_SAVER
#define FFW_MEDIA_PNG_SAVER
#include "imagesaver.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef PNG_H
typedef void* png_structp;
typedef void* png_infop;
#endif
#endif

namespace ffw {
    /**
     * @ingroup media
     */
    class FFW_API PngSaver: public ImageWriter {
    public:
        PngSaver(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);
        PngSaver(const PngSaver& other) = delete;
        PngSaver(PngSaver&& other);
        void swap(PngSaver& other);
        PngSaver& operator = (const PngSaver& other) = delete;
        PngSaver& operator = (PngSaver&& other);
        virtual ~PngSaver();

        size_t writeRow(const void* src) override;
        bool writeFooter() override;
        void close() override;

    private:
        void open(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);

        png_structp pngPtr;
        png_infop infoPtr;
        FILE* output;
        std::string errorStr;
    };
}

inline void swap(ffw::PngSaver& first, ffw::PngSaver& second){
    first.swap(second);
}

#endif