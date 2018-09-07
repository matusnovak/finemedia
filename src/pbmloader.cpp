/*** This file is part of FineFramework project ***/

#include "ffw/media/pbmloader.h"
#include "ffw/media/byteswap.h"
#include "ffw/media/exceptions.h"

///=============================================================================
ffw::PbmLoader::PbmLoader(const std::string& path){
    open(path);
}

///=============================================================================
ffw::PbmLoader::PbmLoader(PbmLoader&& other){
    swap(other);
}

///=============================================================================
void ffw::PbmLoader::swap(PbmLoader& other){
    std::swap(input, other.input);
}

///=============================================================================
ffw::PbmLoader& ffw::PbmLoader::operator = (PbmLoader&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::PbmLoader::~PbmLoader(){
    close();
}

///=============================================================================
void ffw::PbmLoader::open(const std::string& path){

    input->open(path, std::ios::in | std::ios::binary);

    if (!input->is_open()) throw ImageException::invalidFile();

    std::string header[4];
    
    for(int i = 0; i < 4; i++){
        if(!std::getline(*input, header[i])){
            close();
            throw ImageException::invalidHeader();
        }

        if(header[i].size() == 0){
            close();
            throw ImageException::invalidHeader();
        }
    }

    try{
        width = std::stoll(header[1]);
        height = std::stoll(header[2]);
        depth = 0;
        mipmaps = 1;
    } catch(std::exception e){
        close();
        throw ImageException::invalidHeader();
    }

    size_t offset = (size_t)input->tellg();
    input->seekg(0, std::ios::end);
    size_t size = (size_t)input->tellg();
    input->seekg(offset);
    size_t dataLength = 0;

    if(header[0] == "P6" && header[3] == "255"){
        format = ffw::ImageType::RGB_888;
        dataLength = width*height*3;

    } else if(header[0] == "P6" && header[3] == "65535"){
        format = ffw::ImageType::RGB_161616;
        dataLength = width*height*6;

    } else if(header[0] == "PF" && header[3] == "-1.0000"){
        format = ffw::ImageType::RGB_323232F;
        dataLength = width*height*12;

    } else if(header[0] == "P5" && header[3] == "255"){
        format = ffw::ImageType::GRAYSCALE_8;
        dataLength = width*height;

    } else if(header[0] == "P5" && header[3] == "65535"){
        format = ffw::ImageType::GRAYSCALE_16;
        dataLength = width*height*2;

    } else if(header[0] == "Pf" && header[3] == "-1.0000"){
        format = ffw::ImageType::GRAYSCALE_32F;
        dataLength = width*height*4;

    } else {
        close();
        throw ImageException::invalidFormat();
    }

    if(dataLength + offset > size){
        close();
        throw ImageException::invalidFileSize();
    }

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::PbmLoader::close(){
    input->close();
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
size_t ffw::PbmLoader::readRow(void* dest){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(dest == NULL)return 0;
    
    switch(format){
        case ffw::ImageType::GRAYSCALE_8: {
            input->read((char*)dest, width);
            break;
        }
        case ffw::ImageType::GRAYSCALE_16: {
            input->read((char*)dest, width*2);
            unsigned short* ptr = (unsigned short*)dest;
            for(size_t i = 0; i < (size_t)width; i++){
                ptr[i] = byteSwap16(ptr[i]);
            }
            break;
        }
        case ffw::ImageType::GRAYSCALE_32F: {
            input->read((char*)dest, width*4);
            break;
        }
        case ffw::ImageType::RGB_888: {
            input->read((char*)dest, width*3);
            break;
        }
        case ffw::ImageType::RGB_161616: {
            input->read((char*)dest, width*2*3);
            unsigned short* ptr = (unsigned short*)dest;
            for(size_t i = 0; i < (size_t)width*3; i++){
                ptr[i] = byteSwap16(ptr[i]);
            }
            break;
        }
        case ffw::ImageType::RGB_323232F: {
            input->read((char*)dest, width*4*3);
            break;
        }
        default: {
            return 0;
        }
    }
    row++;

    return this->getStrideSize();
}