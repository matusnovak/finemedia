/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_DDS_SAVER
#define FFW_MEDIA_DDS_SAVER
#include "imagesaver.h"
#include <fstream>

namespace ffw {
    /**
     * @ingroup media
     */
    class FFW_API DdsSaver: public ImageWriter {
    public:
        DdsSaver(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);
        DdsSaver(const DdsSaver& other) = delete;
        DdsSaver(DdsSaver&& other);
        void swap(DdsSaver& other);
        DdsSaver& operator = (const DdsSaver& other) = delete;
        DdsSaver& operator = (DdsSaver&& other);
        virtual ~DdsSaver();

        size_t writeRow(const void* src) override;
        bool writeFooter() override;
        void close() override;
        bool setMipMap(int level) override;

    private:
        void open(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);

        ffw::SwapWrapper<std::fstream> output;
        size_t pixelsOffset;
    };
}

inline void swap(ffw::DdsSaver& first, ffw::DdsSaver& second){
    first.swap(second);
}

#endif