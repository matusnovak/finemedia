/*** This file is part of FineFramework project ***/

#include "ffw/media/tgasaver.h"
#include "ffw/media/byteswap.h"
#include "ffw/media/exceptions.h"

static const char tgaFooter[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x52, 0x55, 0x45, 0x56, 0x49, 0x53, 0x49, 0x4F, 0x4E, 0x2D, 0x58, 0x46, 0x49, 0x4c, 0x45, 0x2E, 0x00};

///=============================================================================
ffw::TgaSaver::TgaSaver(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    pixelsOffset = 0;
    open(path, w, h, type, quality, mips);
}

///=============================================================================
ffw::TgaSaver::TgaSaver(TgaSaver&& other){
    swap(other);
}

///=============================================================================
void ffw::TgaSaver::swap(TgaSaver& other){
    std::swap(output, other.output);
    std::swap(pixelsOffset, other.pixelsOffset);
}

///=============================================================================
ffw::TgaSaver& ffw::TgaSaver::operator = (TgaSaver&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::TgaSaver::~TgaSaver(){
    close();
}

///=============================================================================
void ffw::TgaSaver::open(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    (void)quality;
    (void)mips;

    if (w <= 0 || h <= 0)throw ImageException::invalidWidthHeight();

    switch(type){
        case ImageType::GRAYSCALE_8:
        case ImageType::RGB_ALPHA_5551:
        case ImageType::RGB_ALPHA_8888:
        case ImageType::RGB_888:
            break;
        default:
            throw ImageException::unsupportedImageType();
    }

    output->open(path, std::ios::trunc | std::ios::out | std::ios::binary);

    if(!output->is_open()){
        throw ImageException::invalidFile();
    }

    uint8_t idLength = 0;
    uint8_t colorMapType = 0;
    uint8_t imageType = 3;
    uint16_t colorMapEntry = 0;
    uint16_t colorMapLength = 0;
    uint8_t colorMapSize = 0;
    uint16_t originX = 0;
    uint16_t originY = 0;
    uint16_t ws = static_cast<uint16_t>(w);
    uint16_t hs = static_cast<uint16_t>(h);
    uint8_t bitsPerPixel = 32;
    uint8_t imageDescriptor = 0;

    switch(type){
        case ffw::ImageType::GRAYSCALE_8: {
            imageType = 3;
            bitsPerPixel = 8;
            imageDescriptor = 0;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_5551: {
            imageType = 2;
            bitsPerPixel = 16;
            imageDescriptor = 1;
            break;
        }
        case ffw::ImageType::RGB_888: {
            imageType = 2;
            bitsPerPixel = 24;
            imageDescriptor = 0;
            break;
        }
        case ffw::ImageType::RGB_ALPHA_8888: {
            imageType = 2;
            bitsPerPixel = 32;
            imageDescriptor = 8;
            break;
        }
        default: {
            close();
            throw ImageException::unsupportedImageType();
        }
    }

    // Write header
    output->write((char*)&idLength,           sizeof(uint8_t));
    output->write((char*)&colorMapType,       sizeof(uint8_t));
    output->write((char*)&imageType,          sizeof(uint8_t));
    output->write((char*)&colorMapEntry,      sizeof(uint16_t));
    output->write((char*)&colorMapLength,     sizeof(uint16_t));
    output->write((char*)&colorMapSize,       sizeof(uint8_t));
    output->write((char*)&originX,            sizeof(uint16_t));
    output->write((char*)&originY,            sizeof(uint16_t));
    output->write((char*)&ws,                 sizeof(uint16_t));
    output->write((char*)&hs,                 sizeof(uint16_t));
    output->write((char*)&bitsPerPixel,       sizeof(uint8_t));
    output->write((char*)&imageDescriptor,    sizeof(uint8_t));

    format = type;
    width = w;
    height = h;
    depth = 0;
    mipmaps = 1;

    pixelsOffset = (size_t)output->tellg();

    for(size_t i = 0; i < height * getStrideSize(); i++){
        static const char data = 0x00;
        output->write(&data, 1);
    }

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::TgaSaver::close(){
    output->close();
    width = 0;
    height = 0;
    loaded = 0;
    row = 0;
    depth = 0;
    mipmaps = 0;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::TgaSaver::writeRow(const void* src){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(src == NULL)return 0;

    output->seekg(pixelsOffset + (height - row - 1) * getStrideSize());
    
    switch(format){
        case ffw::ImageType::GRAYSCALE_8: {
            output->write((char*)src, width);
            break;
        }
        case ffw::ImageType::RGB_ALPHA_5551: {

            const char* ptr = (const char*)src;
            for(size_t i = 0; i < (size_t)width*2; i += 2){
                output->write(&ptr[i + 1], 1);
                output->write(&ptr[i + 0], 1);
            }
            break;
        }
        case ffw::ImageType::RGB_888: {

            const char* ptr = (const char*)src;
            for(size_t i = 0; i < (size_t)width*3; i += 3){
                output->write(&ptr[i + 2], 1);
                output->write(&ptr[i + 1], 1);
                output->write(&ptr[i + 0], 1);
            }
            break;
        }
        case ffw::ImageType::RGB_ALPHA_8888: {

            const char* ptr = (const char*)src;
            for(size_t i = 0; i < (size_t)width*4; i += 4){
                output->write(&ptr[i + 2], 1);
                output->write(&ptr[i + 1], 1);
                output->write(&ptr[i + 0], 1);
                output->write(&ptr[i + 3], 1);
            }
            break;
        }
    }
    row++;

    return this->getStrideSize();
}

///=============================================================================
bool ffw::TgaSaver::writeFooter(){
    if(!loaded)return false;
    if(row != height)return false;

    output->write(&tgaFooter[0], sizeof(tgaFooter));

    return true;
}