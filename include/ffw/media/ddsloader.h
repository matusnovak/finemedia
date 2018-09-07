/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_DDS_LOADER
#define FFW_MEDIA_DDS_LOADER
#include "imageloader.h"
#include <fstream>

namespace ffw {
    /**
     * @ingroup media
     */
    class FFW_API DdsLoader: public ImageReader {
    public:
        DdsLoader(const std::string& path);
        DdsLoader(const DdsLoader& other) = delete;
        DdsLoader(DdsLoader&& other);
        void swap(DdsLoader& other);
        DdsLoader& operator = (const DdsLoader& other) = delete;
        DdsLoader& operator = (DdsLoader&& other);
        virtual ~DdsLoader();

        size_t readRow(void* dest) override;
        bool getMipMap(int level) override;
        bool getCubemap(int side);
        void close() override;

        inline bool isCubemap() const {
            return iscubemap;
        }
        inline int getCubemapOffset() const {
            return cubemapOffset;
        }
    private:
        void open(const std::string& path);

        ffw::SwapWrapper<std::fstream> input;
        size_t pixelsOffset;
        bool invertBits;
        bool swaprgb;
        bool iscubemap;
        int cubemapOffset;
    };
}

inline void swap(ffw::DdsLoader& first, ffw::DdsLoader& second){
    first.swap(second);
}

#endif