/*** This file is part of FineFramework project ***/

#include "ffw/media/imageloader.h"

///=============================================================================
ffw::ImageReader::ImageReader(){
    format = ImageType::INVALID;
    width = 0;
    height = 0;
    depth = 0;
    loaded = false;
    row = 0;
    mipmaps = 0;
    mipmapOffset = 0;
}

///=============================================================================
ffw::ImageReader::~ImageReader(){
}

///=============================================================================
bool ffw::ImageReader::getMipMap(int level) {
    (void)level;
    return false;
}

///=============================================================================
bool ffw::ImageReader::readAll(void* dest) {
    ImageFormat f(getWidth(), getHeight(), getDepth(), getImageType());

    unsigned char* destPtr = (unsigned char*)dest;

    for (int i = 0; i <= getNumOfMipMaps(); i++) {
        for (int y = 0; y < getHeight(i); y++) {
            void* ptr = &(destPtr + f.getMipMapPtrOffset(i))[getRowOffset() * getStrideSize(i)];
            if (!eof() && readRow(ptr)) {
                // Read row ok
            }
            else {
                return false;
            }
        }

        // Get next mip map test
        if (!getMipMap(i+1))break;
    }

    return true;
}

///=============================================================================
bool ffw::ImageReader::readAll(ffw::ImageBuffer& image) {
    if(!image.allocate(getWidth(), getHeight(), getImageType(), NULL, getNumOfMipMaps()))return false;

    unsigned char* dest = image.getPtr();

    return readAll(dest);
}