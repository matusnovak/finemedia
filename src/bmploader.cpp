/*** This file is part of FineFramework project ***/

#include "ffw/media/bmploader.h"
#include "ffw/media/exceptions.h"

#define SWAP_4BITS(c) ((c & 0xf0) >> 4) | ((c & 0x0f) << 4);

///=============================================================================
ffw::BmpLoader::BmpLoader(const std::string& path):input(){
    invertBits = false;
    open(path);
}

///=============================================================================
ffw::BmpLoader::BmpLoader(BmpLoader&& other){
    swap(other);
}

///=============================================================================
void ffw::BmpLoader::swap(BmpLoader& other){
    std::swap(input, other.input);
    std::swap(pixelsOffset, other.pixelsOffset);
    std::swap(invertBits, other.invertBits);
}

///=============================================================================
ffw::BmpLoader& ffw::BmpLoader::operator = (BmpLoader&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::BmpLoader::~BmpLoader(){
    close();
}

///=============================================================================
void ffw::BmpLoader::open(const std::string& path){
    input->open(path, std::ios::in | std::ios::binary);

    if(!input->is_open()){
        throw ImageException::invalidFile();
    }

    // Check if file is big enough to contain header
    input->seekg(0, std::ios::end);
    size_t size = (size_t)input->tellg();
    input->seekg(0, std::ios::beg);
    if(size < 54)throw ImageException::invalidHeader();

    uint16_t fileType;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t infoSize;
    //uint32_t width;
    //uint32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t resolutionX;
    uint32_t resolutionY;
    uint32_t numberOfColors;
    uint32_t importantColors;
    //imageType type;

    input->read((char*)&fileType, sizeof(uint16_t));
    if(fileType != 0x4D42){
        throw ImageException("Invalid file type expected 0x4D42");
    }

    input->read((char*)&fileSize,           sizeof(uint32_t));
    input->read((char*)&reserved,           sizeof(uint32_t));
    input->read((char*)&dataOffset,         sizeof(uint32_t));
    input->read((char*)&infoSize,           sizeof(uint32_t));
    input->read((char*)&width,              sizeof(uint32_t));
    input->read((char*)&height,             sizeof(uint32_t));
    input->read((char*)&planes,             sizeof(uint16_t));
    input->read((char*)&bitsPerPixel,       sizeof(uint16_t));
    input->read((char*)&compression,        sizeof(uint32_t));
    input->read((char*)&imageSize,          sizeof(uint32_t));
    input->read((char*)&resolutionX,        sizeof(uint32_t));
    input->read((char*)&resolutionY,        sizeof(uint32_t));
    input->read((char*)&numberOfColors,     sizeof(uint32_t));
    input->read((char*)&importantColors,    sizeof(uint32_t));
    depth = 0;
    mipmaps = 1;

    if (planes != 1)throw ImageException("Invalid number of planes expected one");

    format = ffw::ImageType::INVALID;

    if(bitsPerPixel == 16 && compression == 3) {
        format = ffw::ImageType::RGB_ALPHA_4444;
    } else if(compression == 0){
        if(bitsPerPixel == 1){
            format = ffw::ImageType::BITMAP_1;

        } else if(bitsPerPixel == 4){
            format = ffw::ImageType::GRAYSCALE_4;

        } else if(bitsPerPixel == 8){
            format = ffw::ImageType::GRAYSCALE_8;

        } else if(bitsPerPixel == 24){
            format = ffw::ImageType::RGB_888;

        } else if(bitsPerPixel == 32){
            format = ffw::ImageType::RGB_ALPHA_8888;

        }
    }

    if (format == ffw::ImageType::INVALID) throw ImageException::invalidFormat();

    // Check if file contains all pixels
    if(size < 54 + size_t(width*height*(bitsPerPixel/8))){
        throw ImageException::invalidFileSize();
    }

    if(dataOffset > 54){
        uint8_t* additionalInfo = new uint8_t[dataOffset-54];
        input->read((char*)additionalInfo, dataOffset-54);

        if(dataOffset-54 == 8 && format == ffw::ImageType::BITMAP_1){
            if(additionalInfo[0] == 0xFF && additionalInfo[4] == 0x00){
                invertBits = true;
            }
        }

        delete[] additionalInfo;
    }

    pixelsOffset = (size_t)input->tellg();

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::BmpLoader::close(){
    input->close();
    width = 0;
    height = 0;
    depth = 0;
    mipmaps = 0;
    mipmapOffset = 0;
    loaded = 0;
    row = 0;
    invertBits = false;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::BmpLoader::readRow(void* dest){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(dest == NULL)return 0;
    
    size_t size = 0;
    int offset = 4 - int(width * (getBitsPerPixel() / 8.0f)) % 4;
    if(format == ffw::ImageType::RGB_ALPHA_8888)offset = 0;
    int scanline = int(width * (getBitsPerPixel() / 8.0f) + offset);
    int rowOffset = height - row - 1;

    switch(format){
        case ffw::ImageType::BITMAP_1: {
            size = (width) /8;
            if(width % 8 > 0)size++;

            input->seekg(pixelsOffset + rowOffset * scanline);
            input->read((char*)dest, size);

            if(invertBits){
                unsigned char* ptr = (unsigned char*)dest;
                for(size_t i = 0; i < size; i++)ptr[i] = ~ptr[i];
            }
            
            break;
        }

        case ffw::ImageType::GRAYSCALE_4: {
            size = (width) /2;
            if(width % 2 > 0)size++;

            input->seekg(pixelsOffset + rowOffset * scanline);
            input->read((char*)dest, size);
            
            break;
        }

        case ffw::ImageType::GRAYSCALE_8: {
            input->seekg(pixelsOffset + rowOffset * scanline);
            input->read((char*)dest, width);
            
            break;
        }

        case ffw::ImageType::RGB_ALPHA_4444: {
            input->seekg(pixelsOffset + rowOffset * scanline);
            input->read((char*)dest, width * 2);

            unsigned char* ptr = (unsigned char*)dest;
            for(size_t i = 0; i < size_t(width)*2; i += 2) {
                auto t = ptr[i + 0];
                ptr[i + 0] = ((t & 0xf0) >> 4) | ((t & 0x0f) << 4);

                t = ptr[i + 1];
                ptr[i + 1] = ((t & 0xf0) >> 4) | ((t & 0x0f) << 4);

                std::swap(ptr[i + 0], ptr[i + 1]);

                t = ptr[i + 1] & 0xF0;
                ptr[i + 1] = (ptr[i + 1] & 0x0F) | (ptr[i + 0] & 0xF0);
                ptr[i + 0] = (ptr[i + 0] & 0x0F) | t;
            }
            break;
        }
        
        case ffw::ImageType::RGB_888: {
            input->seekg(pixelsOffset + rowOffset * scanline);

            input->read((char*)dest, width * 3);
            unsigned char* ptr = (unsigned char*)dest;
            for(size_t i = 0; i < size_t(width)*3; i += 3){
                std::swap(ptr[i + 0], ptr[i + 2]);
            }

            break;
        }

        case ffw::ImageType::RGB_ALPHA_8888: {
            input->seekg(pixelsOffset + rowOffset * scanline);
            input->read((char*)dest, width * 4);

            unsigned char* ptr = (unsigned char*)dest;
            for(size_t i = 0; i < size_t(width)*4; i += 4){
                std::swap(ptr[i + 0], ptr[i + 2]);
            }
            
            break;
        }
        default: return 0;
    }
    
    row++;

    return this->getStrideSize();
}