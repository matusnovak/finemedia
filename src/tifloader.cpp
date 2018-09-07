/*** This file is part of FineFramework project ***/
#include <stdio.h>
#include <tiffio.h>
#include "ffw/media/tifloader.h"
#include "ffw/media/exceptions.h"

///=============================================================================
static void DummyHandler(const char* module, const char* fmt, va_list ap){
    // ignore errors and warnings (or handle them your own way)
}

///=============================================================================
ffw::TifLoader::TifLoader(const std::string& path){
    tiff = NULL;
    open(path);
}

///=============================================================================
ffw::TifLoader::TifLoader(TifLoader&& other){
    tiff = NULL;
    swap(other);
}

///=============================================================================
void ffw::TifLoader::swap(TifLoader& other){
    std::swap(tiff, other.tiff);
}

///=============================================================================
ffw::TifLoader& ffw::TifLoader::operator = (TifLoader&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::TifLoader::~TifLoader(){
    close();
}

///=============================================================================
void ffw::TifLoader::open(const std::string& path){
    TIFFSetWarningHandler(DummyHandler);

    tiff = TIFFOpen(path.c_str(), "rb");

    if(!tiff){
        throw ImageException::invalidFile();
    }

    size_t scanline;
    uint32_t w;
    uint32_t h;
    uint16_t bitsPerPixel;
    uint16_t compression;
    uint16_t dataType;
    //imageType type;

    uint16_t photometric;
    uint16_t samplesPerPixel;

    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerPixel);
    TIFFGetField(tiff, TIFFTAG_COMPRESSION, &compression);
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photometric);
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(tiff, TIFFTAG_DATATYPE, &dataType);

    scanline = TIFFScanlineSize(tiff);

    //std::cout << "bitsPerPixel: " << bitsPerPixel << std::endl;
    //std::cout << "samplesPerPixel: " << samplesPerPixel << std::endl;

    switch(bitsPerPixel){
        case 8: {
            switch(samplesPerPixel){
                case 1: {
                    format = ffw::ImageType::GRAYSCALE_8;
                    break;
                }
                case 2: {
                    format = ffw::ImageType::GRAYSCALE_ALPHA_8;
                    break;
                }
                case 3: {
                    format = ffw::ImageType::RGB_888;
                    break;
                }
                case 4: {
                    format = ffw::ImageType::RGB_ALPHA_8888;
                    break;
                }
                default: {
                    close();
                    throw ImageException::invalidFormat();
                }
            }
            break;
        }
        case 16: {
            switch(samplesPerPixel){
                case 1: {
                    format = ffw::ImageType::GRAYSCALE_16;
                    break;
                }
                case 2: {
                    format = ffw::ImageType::GRAYSCALE_ALPHA_16;
                    break;
                }
                case 3: {
                    format = ffw::ImageType::RGB_161616;
                    break;
                }
                case 4: {
                    format = ffw::ImageType::RGB_ALPHA_16161616;
                    break;
                }
                default: {
                    close();
                    throw ImageException::invalidFormat();
                }
            }
            break;
        }
        case 32: {
            switch(samplesPerPixel){
                case 1: {
                    format = ffw::ImageType::GRAYSCALE_32F;
                    break;
                }
                case 2: {
                    format = ffw::ImageType::GRAYSCALE_ALPHA_32F;
                    break;
                }
                case 3: {
                    format = ffw::ImageType::RGB_323232F;
                    break;
                }
                case 4: {
                    format = ffw::ImageType::RGB_ALPHA_32323232F;
                    break;
                }
                default: {
                    close();
                    throw ImageException::invalidFormat();
                }
            }
            break;
        }
        default: {
            close();
            throw ImageException::invalidFormat();
        }
    }

    width = w;
    height = h;
    depth = 0;
    mipmaps = 1;

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::TifLoader::close(){
    if(tiff != NULL){
        TIFFClose(tiff);
    }
    tiff = NULL;
    width = 0;
    height = 0;
    loaded = 0;
    row = 0;
    depth = 0;
    mipmaps = 0;
    mipmapOffset = 0;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::TifLoader::readRow(void* dest){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(dest == NULL)return 0;

    TIFFReadScanline(tiff, dest, row);
    row++;

    return this->getStrideSize();
}
