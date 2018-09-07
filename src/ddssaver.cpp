/*** This file is part of FineFramework project ***/

#include "ffw/media/ddssaver.h"
#include "ddsheader.h"
#include "ffw/media/exceptions.h"

///=============================================================================
ffw::DdsSaver::DdsSaver(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    open(path, w, h, type, quality, mips);
}

///=============================================================================
ffw::DdsSaver::DdsSaver(DdsSaver&& other){
    swap(other);
}

///=============================================================================
void ffw::DdsSaver::swap(DdsSaver& other){
    std::swap(output, other.output);
    std::swap(pixelsOffset, other.pixelsOffset);
}

///=============================================================================
ffw::DdsSaver& ffw::DdsSaver::operator = (DdsSaver&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::DdsSaver::~DdsSaver(){
    close();
}

///=============================================================================
void ffw::DdsSaver::open(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    (void)quality;
    if (w <= 0 || h <= 0)throw ImageException::invalidWidthHeight();

    switch(type){
        case ffw::ImageType::GRAYSCALE_8:
        case ffw::ImageType::GRAYSCALE_16:
        case ffw::ImageType::GRAYSCALE_16F:
        case ffw::ImageType::GRAYSCALE_32F:
        case ffw::ImageType::GRAYSCALE_ALPHA_8:
        case ffw::ImageType::GRAYSCALE_ALPHA_16:
        case ffw::ImageType::GRAYSCALE_ALPHA_16F:
        case ffw::ImageType::GRAYSCALE_ALPHA_32F:
        case ffw::ImageType::RG_88:
        case ffw::ImageType::RG_1616:
        case ffw::ImageType::RG_1616F:
        case ffw::ImageType::RGB_888:
        case ffw::ImageType::RGB_ALPHA_4444:
        case ffw::ImageType::RGB_ALPHA_8888:
        case ffw::ImageType::RGB_ALPHA_16161616F:
        case ffw::ImageType::RGB_ALPHA_32323232F:
        case ffw::ImageType::RED_RGTC1:
        case ffw::ImageType::RED_GREEN_RGTC2:
        case ffw::ImageType::SIGNED_RED_RGTC1:
        case ffw::ImageType::SIGNED_RED_GREEN_RGTC2:
        case ffw::ImageType::RGB_DXT1:
        case ffw::ImageType::RGB_ALPHA_DXT3:
        case ffw::ImageType::RGB_ALPHA_DXT5:
            break;
        default:
            throw ImageException::unsupportedImageType();
    }

    output->open(path, std::ios::trunc | std::ios::out | std::ios::binary);

    if(!output->is_open()){
        throw ImageException::invalidFile();
    }

    _DDS_HEADER header;
    memset(&header, 0x00, sizeof(_DDS_HEADER));

    header.dwSize = 124;
    header.ddspf.dwSize = 32;
    header.dwMipMapCount = mips;
    header.dwWidth = w;
    header.dwHeight = h;
    header.dwDepth = 0;

    header.dwFlags = _DDS_HEADER_FLAGS_WIDTH | _DDS_HEADER_FLAGS_HEIGHT | _DDS_HEADER_FLAGS_CAPS | _DDS_HEADER_FLAGS_PIXELFORMAT;
    switch(type) {
        case ffw::ImageType::RED_RGTC1:
        case ffw::ImageType::RED_GREEN_RGTC2:
        case ffw::ImageType::SIGNED_RED_RGTC1:
        case ffw::ImageType::SIGNED_RED_GREEN_RGTC2:
        case ffw::ImageType::RGB_DXT1:
        case ffw::ImageType::RGB_ALPHA_DXT3:
        case ffw::ImageType::RGB_ALPHA_DXT5: {
            header.dwFlags |= _DDS_HEADER_FLAGS_LINEARSIZE;
            header.dwFlags |= _DDS_HEADER_FLAGS_DEPTH; // ???
            break;
        }
        default: {
            header.dwFlags |= _DDS_HEADER_FLAGS_LINEARSIZE;
            break;
        }
    }

    if(mips > 1) {
        header.dwFlags |= _DDS_HEADER_FLAGS_MIPS;
    }
    if(depth > 0) {
        header.dwFlags |= _DDS_HEADER_FLAGS_DEPTH;
    }

    if (mips > 1)header.dwCaps = _DDS_HEADER_CAPS1_TEXTURE | _DDS_HEADER_CAPS1_COMPLEX | _DDS_HEADER_CAPS1_MIPMAP;
    else header.dwCaps = _DDS_HEADER_CAPS1_TEXTURE;

    switch(type) {
        case ffw::ImageType::GRAYSCALE_8: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_LUM;
            header.ddspf.dwRGBBitCount = 8;
            header.ddspf.dwRBitMask = 0xff;
            break;
        }
        case ffw::ImageType::GRAYSCALE_16: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_LUM;
            header.ddspf.dwRGBBitCount = 16;
            header.ddspf.dwRBitMask = 0xffff;
            break;
        }
        case ffw::ImageType::GRAYSCALE_16F: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_GRAY_16F;
            break;
        }
        case ffw::ImageType::GRAYSCALE_32F: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_GRAY_32F;
            break;
        }
        case ffw::ImageType::GRAYSCALE_ALPHA_8: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_LUM;
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_ALPHA_PIXELS;
            header.ddspf.dwRGBBitCount = 16;
            header.ddspf.dwRBitMask = 0xff;
            header.ddspf.dwABitMask = 0xff00;
            break;
        }
        case ffw::ImageType::GRAYSCALE_ALPHA_16: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_LUM;
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_ALPHA_PIXELS;
            header.ddspf.dwRGBBitCount = 32;
            header.ddspf.dwRBitMask = 0xffff;
            header.ddspf.dwABitMask = 0xffff0000;
            break;
        }
        case ffw::ImageType::RG_88: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_RGB;
            header.ddspf.dwRGBBitCount = 16;
            header.ddspf.dwRBitMask = 0xff;
            header.ddspf.dwGBitMask = 0xff00;
            break;
        }
        case ffw::ImageType::RG_1616: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_RGB;
            header.ddspf.dwRGBBitCount = 32;
            header.ddspf.dwRBitMask = 0xffff;
            header.ddspf.dwGBitMask = 0xffff0000;
            break;
        }
        case ffw::ImageType::RG_1616F: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_RG_1616F;
            break;
        }
        case ffw::ImageType::RGB_888: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_RGB;
            header.ddspf.dwRGBBitCount = 24;
            header.ddspf.dwRBitMask = 0xff0000;
            header.ddspf.dwGBitMask = 0xff00;
            header.ddspf.dwBBitMask = 0xff;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_4444: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_RGB;
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_ALPHA_PIXELS;
            header.ddspf.dwRGBBitCount = 16;
            header.ddspf.dwRBitMask = 0xf00;
            header.ddspf.dwGBitMask = 0xf0;
            header.ddspf.dwBBitMask = 0xf;
            header.ddspf.dwABitMask = 0xf000;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_8888: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_RGB;
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_ALPHA_PIXELS;
            header.ddspf.dwRGBBitCount = 32;
            header.ddspf.dwRBitMask = 0xff0000;
            header.ddspf.dwGBitMask = 0xff00;
            header.ddspf.dwBBitMask = 0xff;
            header.ddspf.dwABitMask = 0xff000000;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_16161616F: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_RGB_ALPHA_16161616F;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_32323232F: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_RGB_ALPHA_32323232F;
            break;
        }
        case ffw::ImageType::RED_RGTC1: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_RED_RGTC1;
            header.dwDepth = 1;
            break;
        }
        case ffw::ImageType::RED_GREEN_RGTC2: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_RED_GREEN_RGTC2;
            header.dwDepth = 1;
            break;
        }
        case ffw::ImageType::SIGNED_RED_RGTC1: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_SIGNED_RED_RGTC1;
            header.dwDepth = 1;
            break;
        }
        case ffw::ImageType::SIGNED_RED_GREEN_RGTC2: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_SIGNED_RED_GREEN_RGTC2;
            header.dwDepth = 1;
            break;
        }
        case ffw::ImageType::RGB_DXT1: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_DXT1;
            header.dwDepth = 1;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_DXT3: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_DXT3;
            header.dwDepth = 1;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_DXT5: {
            header.ddspf.dwFlags |= _DDS_PIXELFORMAT_FLAGS_COMPRESSED;
            header.ddspf.dwFourCC = _DDS_FOURCC_DXT5;
            header.dwDepth = 1;
            break;
        }
        default: {
            close();
            throw ImageException::unsupportedImageType();
        }
    }

    width = w;
    height = h;
    depth = 0;
    format = type;
    row = 0;
    mipmaps = mips;

    header.dwPitchOrLinearSize = getAllocationSize();

    output->write("DDS ", 4);
    output->write((char*)&header, sizeof(_DDS_HEADER));

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::DdsSaver::close(){
    output->close();
    width = 0;
    height = 0;
    loaded = 0;
    depth = 0;
    mipmaps = 0;
    row = 0;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::DdsSaver::writeRow(const void* src){
    if(!loaded)return 0;
    if (row >= getHeight(mipmapOffset)) {
        return 0;
    }
    if(src == NULL)return 0;

    switch (format) {
        case ffw::ImageType::GRAYSCALE_8:
        case ffw::ImageType::GRAYSCALE_16:
        case ffw::ImageType::GRAYSCALE_16F:
        case ffw::ImageType::GRAYSCALE_32F:
        case ffw::ImageType::GRAYSCALE_ALPHA_8:
        case ffw::ImageType::GRAYSCALE_ALPHA_16:
        case ffw::ImageType::GRAYSCALE_ALPHA_16F:
        case ffw::ImageType::GRAYSCALE_ALPHA_32F:
        case ffw::ImageType::RG_88:
        case ffw::ImageType::RG_1616:
        case ffw::ImageType::RG_1616F:
        case ffw::ImageType::RGB_ALPHA_16161616:
        case ffw::ImageType::RGB_ALPHA_16161616F:
        case ffw::ImageType::RGB_ALPHA_32323232F:
        case ffw::ImageType::RED_RGTC1:
        case ffw::ImageType::RED_GREEN_RGTC2:
        case ffw::ImageType::SIGNED_RED_RGTC1:
        case ffw::ImageType::SIGNED_RED_GREEN_RGTC2:
        case ffw::ImageType::RGB_DXT1:
        case ffw::ImageType::RGB_ALPHA_DXT3:
        case ffw::ImageType::RGB_ALPHA_DXT5: {
            if (!output->write((const char*)src, getStrideSize(mipmapOffset)))return false;
            break;
        }
        case ffw::ImageType::RGB_888:
        case ffw::ImageType::RGB_ALPHA_8888: {
            const char* ptr = (const char*)src;
            for(size_t i = 0; i < getWidth(mipmapOffset) * getNumberOfChannels(); i += getNumberOfChannels()){
                //std::swap(ptr[i + 0], ptr[i + 2]);
                output->write(&ptr[i + 2], 1);
                output->write(&ptr[i + 1], 1);
                output->write(&ptr[i + 0], 1);
                if(getNumberOfChannels() > 3) {
                    output->write(&ptr[i + 3], 1);
                }
            }
            break;
        }
        case ffw::ImageType::RGB_ALPHA_4444: {
            const unsigned char* ptr = (const unsigned char*)src;
            for(size_t i = 0; i < getWidth(mipmapOffset) * 2; i += 2){
                const char t0 = ((ptr[i + 1] & 0x0F) << 4) | ((ptr[i + 0] & 0xF0) >> 4);
                output->write(&t0, 1);

                const char t1 = ((ptr[i + 0] & 0x0F) << 4) | ((ptr[i + 1] & 0xF0) >> 4);
                output->write(&t1, 1);
            }
            break;
        }
        default: {
            break;
        }
    }

    row++;

    return this->getStrideSize(mipmapOffset);
}

///=============================================================================
bool ffw::DdsSaver::writeFooter(){
    if(!loaded)return false;
    if(row != height)return false;

    return true;
}

///=============================================================================
bool ffw::DdsSaver::setMipMap(int level) {
    if(level < 0 || level >= mipmaps) {
        return false;
    }

    if(row == getHeight(mipmapOffset)) {
        mipmapOffset = level;
        row = 0;
        return true;
    }
    return false;
}
