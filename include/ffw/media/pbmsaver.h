/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_PBM_SAVER
#define FFW_MEDIA_PBM_SAVER
#include "imagesaver.h"
#include <fstream>

namespace ffw {
    /**
     * @ingroup media
     */
    class FFW_API PbmSaver: public ImageWriter {
    public:
        PbmSaver(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);
        PbmSaver(const PbmSaver& other) = delete;
        PbmSaver(PbmSaver&& other);
        void swap(PbmSaver& other);
        PbmSaver& operator = (const PbmSaver& other) = delete;
        PbmSaver& operator = (PbmSaver&& other);
        virtual ~PbmSaver();

        size_t writeRow(const void* src) override;
        bool writeFooter() override;
        void close() override;

    private:
        void open(const std::string& path, int width, int height, ffw::ImageType type, int quality = 100, int mips = 1);

        ffw::SwapWrapper<std::fstream> output;
    };
}

inline void swap(ffw::PbmSaver& first, ffw::PbmSaver& second){
    first.swap(second);
}

#endif