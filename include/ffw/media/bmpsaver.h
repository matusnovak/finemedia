/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_BMP_SAVER
#define FFW_MEDIA_BMP_SAVER
#include "imagesaver.h"
#include <fstream>

namespace ffw {
    /**
     * @ingroup media
     */
    class FFW_API BmpSaver: public ImageWriter {
    public:
        BmpSaver(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);
        BmpSaver(const BmpSaver& other) = delete;
        BmpSaver(BmpSaver&& other);
        void swap(BmpSaver& other);
        BmpSaver& operator = (const BmpSaver& other) = delete;
        BmpSaver& operator = (BmpSaver&& other);
        virtual ~BmpSaver();

        size_t writeRow(const void* src) override;
        bool writeFooter() override;
        void close() override;

    private:
        void open(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);

        ffw::SwapWrapper<std::fstream> output;
        size_t pixelsOffset;
    };
}

inline void swap(ffw::BmpSaver& first, ffw::BmpSaver& second){
    first.swap(second);
}

#endif