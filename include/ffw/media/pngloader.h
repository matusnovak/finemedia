/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_PNG_LOADER
#define FFW_MEDIA_PNG_LOADER
#include "imageloader.h"

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
    class FFW_API PngLoader: public ImageReader {
    public:
        PngLoader(const std::string& path);
        PngLoader(const PngLoader& other) = delete;
        PngLoader(PngLoader&& other);
        void swap(PngLoader& other);
        PngLoader& operator = (const PngLoader& other) = delete;
        PngLoader& operator = (PngLoader&& other);
        virtual ~PngLoader();

        size_t readRow(void* dest) override;
        void close() override;

    private:
        void open(const std::string& path);

        png_structp pngPtr;
        png_infop infoPtr;
        FILE* input;
        std::string errorStr;
    };
}

inline void swap(ffw::PngLoader& first, ffw::PngLoader& second){
    first.swap(second);
}

#endif