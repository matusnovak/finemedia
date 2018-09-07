/* This file is part of FineFramework project */
#ifndef FFW_MEDIA_JPG_LOADER
#define FFW_MEDIA_JPG_LOADER
#include "imageloader.h"

namespace ffw {
    /**
     * @ingroup media
     */
    class FFW_API JpgLoader: public ImageReader {
    public:
        JpgLoader(const std::string& path);
        JpgLoader(const JpgLoader& other) = delete;
        JpgLoader(JpgLoader&& other);
        void swap(JpgLoader& other);
        JpgLoader& operator = (const JpgLoader& other) = delete;
        JpgLoader& operator = (JpgLoader&& other);
        virtual ~JpgLoader();

        size_t readRow(void* dest) override;
        void close() override;

    private:
        void open(const std::string& path);

        struct jpg_struct_s;
        jpg_struct_s* jpg_struct;
        bool cInfoInit;
        bool decompressInit;
        FILE* input;
    };
}

inline void swap(ffw::JpgLoader& first, ffw::JpgLoader& second){
    first.swap(second);
}

#endif