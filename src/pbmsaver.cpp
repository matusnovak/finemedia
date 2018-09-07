/*** This file is part of FineFramework project ***/

#include "ffw/media/pbmsaver.h"
#include "ffw/media/byteswap.h"
#include "ffw/media/exceptions.h"

///=============================================================================
ffw::PbmSaver::PbmSaver(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    open(path, w, h, type, quality, mips);
}

///=============================================================================
ffw::PbmSaver::PbmSaver(PbmSaver&& other){
    swap(other);
}

///=============================================================================
void ffw::PbmSaver::swap(PbmSaver& other){
    std::swap(output, other.output);
}

///=============================================================================
ffw::PbmSaver& ffw::PbmSaver::operator = (PbmSaver&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::PbmSaver::~PbmSaver(){
    close();
}

///=============================================================================
void ffw::PbmSaver::open(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    (void)quality;
    (void)mips;

    if(w <= 0 || h <= 0)throw ImageException::invalidWidthHeight();

    switch(type){
        case ImageType::GRAYSCALE_8:
        case ImageType::GRAYSCALE_16:
        case ImageType::GRAYSCALE_32F:
        case ImageType::RGB_888:
        case ImageType::RGB_161616:
        case ImageType::RGB_323232F:
            break;
        default:
            ImageException::unsupportedImageType();
    }

    output->open(path, std::ios::trunc | std::ios::out | std::ios::binary);

    if(!output->is_open()){
        throw ImageException::invalidFile();
    }

    std::string header[4];

    header[1] = std::to_string(w) + "\n";
    header[2] = std::to_string(h) + "\n";

    switch(type){
        case ffw::ImageType::GRAYSCALE_8: {
            header[0] = "P5\n";
            header[3] = "255\n";
            break;
        }
        case ffw::ImageType::GRAYSCALE_16: {
            header[0] = "P5\n";
            header[3] = "65535\n";
            break;
        }
        case ffw::ImageType::GRAYSCALE_32F: {
            header[0] = "Pf\n";
            header[3] = "-1.0000\n";
            break;
        }
        case ffw::ImageType::RGB_161616: {
            header[0] = "P6\n";
            header[3] = "65535\n";
            break;
        }
        case ffw::ImageType::RGB_323232F: {
            header[0] = "PF\n";
            header[3] = "-1.0000\n";
            break;
        }
        default: {
            header[0] = "P6\n";
            header[3] = "255\n";
            break;
        }
    }

    format = type;
    width = w;
    height = h;
    depth = 0;
    mipmaps = 1;

    output->write(header[0].c_str(), header[0].size());
    output->write(header[1].c_str(), header[1].size());
    output->write(header[2].c_str(), header[2].size());
    output->write(header[3].c_str(), header[3].size());

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::PbmSaver::close(){
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
size_t ffw::PbmSaver::writeRow(const void* src){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(src == NULL)return 0;
    
    switch(format){
        case ffw::ImageType::GRAYSCALE_8: {
            output->write((const char*)src, width);
            break;
        }
        case ffw::ImageType::GRAYSCALE_16: {
            const unsigned short* ptr = (const unsigned short*)src;
            for(size_t i = 0; i < (size_t)width; i++){
                unsigned short pixel = byteSwap16(ptr[i]);
                output->write((char*)&pixel, sizeof(unsigned short));
            }
            break;
        }
        case ffw::ImageType::GRAYSCALE_32F: {
            output->write((const char*)src, width*4);
            break;
        }
        case ffw::ImageType::RGB_888: {
            output->write((const char*)src, width*3);
            break;
        }
        case ffw::ImageType::RGB_161616: {
            const unsigned short* ptr = (const unsigned short*)src;
            for(size_t i = 0; i < (size_t)width*3; i++){
                unsigned short pixel = byteSwap16(ptr[i]);
                output->write((char*)&pixel, sizeof(unsigned short));
            }
            break;
        }
        case ffw::ImageType::RGB_323232F: {
            output->write((const char*)src, width*4*3);
            break;
        }
    }
    row++;

    return this->getStrideSize();
}

///=============================================================================
bool ffw::PbmSaver::writeFooter(){
    if(!loaded)return false;
    if(row != height)return false;

    

    return true;
}