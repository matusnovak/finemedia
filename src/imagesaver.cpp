/*** This file is part of FineFramework project ***/

#include "ffw/media/imagesaver.h"

///=============================================================================
ffw::ImageWriter::ImageWriter(){
    format = ImageType::INVALID;
    width = 0;
    height = 0;
    loaded = false;
    depth = 0;
    row = 0;
    mipmaps = 0;
    mipmapOffset = 0;
}

///=============================================================================
ffw::ImageWriter::~ImageWriter(){
}

///=============================================================================
bool ffw::ImageWriter::setMipMap(int level) {
    (void)level;
    return false;
}

///=============================================================================
bool ffw::ImageWriter::writeAll(const unsigned char* src) {
    ImageFormat f(getWidth(), getHeight(), getDepth(), getImageType());

    for (int i = 0; i <= getNumOfMipMaps(); i++) {
        for (int y = 0; y < getHeight(i); y++) {
            const auto ptr = &(src + f.getMipMapPtrOffset(i))[getRowOffset() * getStrideSize(i)];
            if (!eof() && writeRow(ptr)) {
                // Write row ok
            }
            else {
                return false;
            }
        }

        // Get next mip map test
        if (!setMipMap(i+1))break;
    }

    writeFooter();
    return true;
}

///=============================================================================
bool ffw::ImageWriter::writeAll(const ffw::ImageBuffer& image) {
    if (!ImageFormat::operator==(image))return false;
    if (!image.isAllocated())return false;
    
    const unsigned char* src = image.getPtr();

    return writeAll(src);
}