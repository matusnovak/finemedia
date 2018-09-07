/*** This file is part of FineFramework project ***/

#include "ffw/media/ddsloader.h"
#include "ddsheader.h"
#include "ffw/media/exceptions.h"

///=============================================================================
ffw::DdsLoader::DdsLoader(const std::string& path):input(){
    invertBits = false;
    swaprgb = false;
    iscubemap = false;
    cubemapOffset = 0;
    open(path);
}

///=============================================================================
ffw::DdsLoader::DdsLoader(DdsLoader&& other){
    swap(other);
}

///=============================================================================
void ffw::DdsLoader::swap(DdsLoader& other){
    std::swap(input, other.input);
    std::swap(pixelsOffset, other.pixelsOffset);
    std::swap(invertBits, other.invertBits);
    std::swap(swaprgb, other.swaprgb);
    std::swap(iscubemap, other.iscubemap);
    std::swap(cubemapOffset, other.cubemapOffset);
}

///=============================================================================
ffw::DdsLoader& ffw::DdsLoader::operator = (DdsLoader&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::DdsLoader::~DdsLoader(){
    close();
}

///=============================================================================
void ffw::DdsLoader::open(const std::string& path){
    input->open(path, std::ios::in | std::ios::binary);

    if(!input->is_open()){
        throw ImageException::invalidFile();
    }

    // Check if file is big enough to contain header
    input->seekg(0, std::ios::end);
    size_t size = (size_t)input->tellg();
    input->seekg(0, std::ios::beg);
    
    if (size < sizeof(_DDS_HEADER)) {
        close();
        throw ImageException::invalidHeader();
    }

    std::string type;
    type.resize(4, '.');
    input->read(&type[0], 4);

    if(type != "DDS ") {
        close();
        throw ImageException::invalidHeader();
    }
    
    _DDS_HEADER header;
    input->read((char*)&header, sizeof(_DDS_HEADER));

    if(header.dwSize != 124) {
        throw ImageException::invalidHeader();
    }

    bool hasMipMaps = (header.dwFlags & _DDS_HEADER_FLAGS_MIPS) != 0;
    bool hasWidth = (header.dwFlags & _DDS_HEADER_FLAGS_WIDTH) != 0;
    bool hasHeight = (header.dwFlags & _DDS_HEADER_FLAGS_HEIGHT) != 0;
    bool hasDepth = (header.dwFlags & _DDS_HEADER_FLAGS_DEPTH) != 0;

    if(!hasWidth || !hasHeight) {
        close();
        throw ImageException("The image is missing width or height from header");
    }

    this->width = header.dwWidth;
    this->height = header.dwHeight;
    this->depth = (header.dwDepth <= 1 ? 0 : header.dwDepth);

    bool hasAlphaPixels = (header.ddspf.dwFlags & _DDS_PIXELFORMAT_FLAGS_ALPHA_PIXELS) != 0;
    bool hasAlpha = (header.ddspf.dwFlags & _DDS_PIXELFORMAT_FLAGS_ALPHA) != 0;
    bool compressed = (header.ddspf.dwFlags & _DDS_PIXELFORMAT_FLAGS_COMPRESSED) != 0;
    bool hasRgb = (header.ddspf.dwFlags & _DDS_PIXELFORMAT_FLAGS_RGB) != 0;
    bool hasLum = (header.ddspf.dwFlags & _DDS_PIXELFORMAT_FLAGS_LUM) != 0;
    this->iscubemap = (header.dwCaps2 & _DDS_HEADER_CAPS2_CUBEMAP) != 0;
    this->mipmaps = header.dwMipMapCount;

    this->format = ffw::ImageType::INVALID;

    if(compressed) {
        switch(header.ddspf.dwFourCC) {
            case _DDS_FOURCC_DXT1: {
                this->format = ffw::ImageType::RGB_DXT1;
                break;
            }
            case _DDS_FOURCC_DXT3: {
                this->format = ffw::ImageType::RGB_ALPHA_DXT3;
                break;
            }
            case _DDS_FOURCC_DXT5: {
                this->format = ffw::ImageType::RGB_ALPHA_DXT5;
                break;
            }
            case _DDS_FOURCC_RED_RGTC1: {
                this->format = ffw::ImageType::RED_RGTC1;
                break;
            }
            case _DDS_FOURCC_RED_GREEN_RGTC2: {
                this->format = ffw::ImageType::RED_GREEN_RGTC2;
                break;
            }
            case _DDS_FOURCC_SIGNED_RED_RGTC1: {
                this->format = ffw::ImageType::SIGNED_RED_RGTC1;
                break;
            }
            case _DDS_FOURCC_SIGNED_RED_GREEN_RGTC2: {
                this->format = ffw::ImageType::SIGNED_RED_GREEN_RGTC2;
                break;
            }
            case _DDS_FOURCC_GRAY_16F: {
                this->format = ffw::ImageType::GRAYSCALE_16F;
                break;
            }
            case _DDS_FOURCC_GRAY_32F: {
                this->format = ffw::ImageType::GRAYSCALE_32F;
                break;
            }
            case _DDS_FOURCC_RG_1616F: {
                this->format = ffw::ImageType::RG_1616F;
                break;
            }
            case _DDS_FOURCC_RGB_ALPHA_16161616F: {
                this->format = ffw::ImageType::RGB_ALPHA_16161616F;
                break;
            }
            case _DDS_FOURCC_RGB_ALPHA_32323232F: {
                this->format = ffw::ImageType::RGB_ALPHA_32323232F;
                break;
            }
            default: {
                break;
            }
        }

    } 
    else if(hasRgb) {
        bool hasRed = header.ddspf.dwRBitMask > 0;
        bool hasGreen = header.ddspf.dwGBitMask > 0;
        bool hasBlue = header.ddspf.dwBBitMask > 0;
        swaprgb = header.ddspf.dwRBitMask > header.ddspf.dwBBitMask;
        switch(header.ddspf.dwRGBBitCount) {
            case 8: {
                this->format = ffw::ImageType::GRAYSCALE_8;
                break;
            }
            case 16: {
                if(hasRed && hasGreen && hasBlue && hasAlphaPixels)this->format = ffw::ImageType::RGB_ALPHA_4444;
                else if(hasRed && hasGreen)this->format = ffw::ImageType::RG_88;
                break;
            }
            case 24: {
                if(hasRed && hasGreen && hasBlue)this->format = ffw::ImageType::RGB_888;
                break;
            }
            case 32: {
                if(hasRed && hasGreen && hasBlue)this->format = ffw::ImageType::RGB_ALPHA_8888;
                else if(hasRed && hasGreen)this->format = ffw::ImageType::RG_1616;
                break;
            }
            default: {
                break;
            }
        }
    }
    else if (hasLum) {
        switch (header.ddspf.dwRGBBitCount) {
            case 8: {
                this->format = ffw::ImageType::GRAYSCALE_8;
                break;
            }
            case 16: {
                if(hasAlphaPixels)this->format = ffw::ImageType::GRAYSCALE_ALPHA_8;
                else this->format = ffw::ImageType::GRAYSCALE_16;
                break;
            }
            case 32: {
                if(hasAlphaPixels)this->format = ffw::ImageType::GRAYSCALE_ALPHA_16;
                break;
            }
            default: {
                break;
            }
        }
    } 
    else if (hasAlpha) {
        switch (header.ddspf.dwRGBBitCount) {
            case 8: {
                this->format = ffw::ImageType::GRAYSCALE_8;
                break;
            }
            case 16: {
                this->format = ffw::ImageType::GRAYSCALE_16;
                break;
            }
            default: {
                break;
            }
        }
    } 

    if(this->format == ffw::ImageType::INVALID) {
        close();
        throw ImageException::invalidFormat();
    }

    pixelsOffset = (size_t)input->tellg();

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::DdsLoader::close(){
    input->close();
    width = 0;
    height = 0;
    loaded = 0;
    row = 0;
    depth = 0;
    mipmaps = 0;
    mipmapOffset = 0;
    cubemapOffset = 0;
    iscubemap = false;
    swaprgb = false;
    invertBits = false;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::DdsLoader::readRow(void* dest){
    if(!loaded)return 0;
    if (row >= getHeight(mipmapOffset)) {
        return 0;
    }
    if(dest == NULL)return 0;

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
            if (!input->read((char*)dest, getStrideSize(mipmapOffset)))return false;
            break;
        }
        case ffw::ImageType::RGB_888:
        case ffw::ImageType::RGB_ALPHA_8888: {
            if (!input->read((char*)dest, getStrideSize(mipmapOffset)))return false;
            if (swaprgb) {
                unsigned char* ptr = (unsigned char*)dest;
                for(size_t i = 0; i < getWidth(mipmapOffset) * getNumberOfChannels(); i += getNumberOfChannels()){
                    std::swap(ptr[i + 0], ptr[i + 2]);
                }
            }
            break;
        }
        case ffw::ImageType::RGB_ALPHA_4444: {
            if (!input->read((char*)dest, getStrideSize(mipmapOffset)))return false;
            if (swaprgb) {
                unsigned char* ptr = (unsigned char*)dest;
                for(size_t i = 0; i < getWidth(mipmapOffset) * 2; i += 2){
                    auto t = ptr[i + 0];
                    ptr[i + 0] = ((t & 0xf0) >> 4) | ((t & 0x0f) << 4);

                    t = ptr[i + 1];
                    ptr[i + 1] = ((t & 0xf0) >> 4) | ((t & 0x0f) << 4);

                    std::swap(ptr[i + 0], ptr[i + 1]);

                    t = ptr[i + 1] & 0xF0;
                    ptr[i + 1] = (ptr[i + 1] & 0x0F) | (ptr[i + 0] & 0xF0);
                    ptr[i + 0] = (ptr[i + 0] & 0x0F) | t;
                }
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
bool ffw::DdsLoader::getMipMap(int level) {
    if(level < 0 || level >= mipmaps) {
        return false;
    }

    if(input->seekg(pixelsOffset + getMipMapPtrOffset(mipmaps) * cubemapOffset + getMipMapPtrOffset(level))) {
        mipmapOffset = level;
        row = 0;
        return true;
    }
    return false;
}

///=============================================================================
bool ffw::DdsLoader::getCubemap(int side) {
    if (!iscubemap)return false;
    if (side < 0 || side >= 6) {
        return false;
    }

    if(input->seekg(pixelsOffset + getMipMapPtrOffset(mipmaps) * side)) {
        mipmapOffset = 0;
        cubemapOffset = side;
        row = 0;
        return true;
    }
    return false;
}