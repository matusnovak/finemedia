/*** This file is part of FineFramework project ***/

#ifndef PNG_LINKAGE_API
#define PNG_LINKAGE_API extern
#endif
#include <png.h>
#include "ffw/media/pngloader.h"
#include "ffw/media/exceptions.h"
#include <csetjmp>

///=============================================================================
static void user_error_fn(png_structp png_ptr, png_const_charp error_msg) {
    auto str = reinterpret_cast<std::string*>(png_get_error_ptr(png_ptr));
    *str = std::string(error_msg);
}
static void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg) {
    (void)png_ptr; 
    (void)warning_msg;
}

///=============================================================================
ffw::PngLoader::PngLoader(const std::string& path){
    pngPtr = NULL;
    infoPtr = NULL;
    input = NULL;
    open(path);
}

///=============================================================================
ffw::PngLoader::PngLoader(PngLoader&& other){
    swap(other);
}

///=============================================================================
void ffw::PngLoader::swap(PngLoader& other){
    std::swap(pngPtr, other.pngPtr);
    std::swap(infoPtr, other.infoPtr);
    std::swap(input, other.input);
    std::swap(errorStr, other.errorStr);
}

///=============================================================================
ffw::PngLoader& ffw::PngLoader::operator = (PngLoader&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::PngLoader::~PngLoader(){
    close();
}

///=============================================================================
void ffw::PngLoader::open(const std::string& path){
    input = fopen(path.c_str(), "rb");

    if(!input){
        throw ImageException::invalidFile();
    }

    uint8_t* header = new uint8_t[8];
    fread(header, 1, 8, input);

    if(png_sig_cmp(header, 0, 8)){
        fclose(input);
        throw ImageException::invalidHeader();
    }

    pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, &this->errorStr, user_error_fn, user_warning_fn);

    if(!pngPtr){
        fclose(input);
        throw ImageException(errorStr.empty() ? "png_create_read_struct error" : errorStr);
    }

    infoPtr = png_create_info_struct(pngPtr);
    if(!infoPtr){
        fclose(input);
        throw ImageException(errorStr.empty() ? "png_create_info_struct error" : errorStr);
    }

    if(setjmp(png_jmpbuf(pngPtr))){
        fclose(input);
        throw ImageException(errorStr.empty() ? "png_jmpbuf error" : errorStr);
    }

    png_init_io(pngPtr, input);
    png_set_sig_bytes(pngPtr, 8);

    png_read_info(pngPtr, infoPtr);

    width = png_get_image_width(pngPtr, infoPtr);
    height = png_get_image_height(pngPtr, infoPtr);
    depth = 0;
    mipmaps = 1;
    png_byte colorType = png_get_color_type(pngPtr, infoPtr);
    png_byte bits = png_get_bit_depth(pngPtr, infoPtr);

    if (bits == 16)
        png_set_swap(pngPtr);

    if(colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pngPtr);

    if(bits == 1){
        if(colorType == PNG_COLOR_TYPE_GRAY){
            format = ImageType::GRAYSCALE_8;
        } else {
            fclose(input);
            throw ImageException::invalidFormat();
        }
    } else if(bits == 4){
        if(colorType == PNG_COLOR_TYPE_GRAY){
            format = ImageType::GRAYSCALE_8;
        } else {
            fclose(input);
            throw ImageException::invalidFormat();
        }

    } else if(bits == 8){
        if(colorType == PNG_COLOR_TYPE_GRAY){
            format = ImageType::GRAYSCALE_8;

        } else if(colorType == PNG_COLOR_TYPE_GRAY_ALPHA){
            format = ImageType::GRAYSCALE_ALPHA_8;

        } else if(colorType == PNG_COLOR_TYPE_RGB){
            format = ImageType::RGB_888;

        } else if(colorType == PNG_COLOR_TYPE_RGB_ALPHA){
            format = ImageType::RGB_ALPHA_8888;

        } else if(colorType == PNG_COLOR_TYPE_PALETTE){
            format = ImageType::RGB_888;

        } else {
            fclose(input);
            throw ImageException::invalidFormat();
        }

    } else if(bits == 16){
        if(colorType == PNG_COLOR_TYPE_GRAY){
            format = ImageType::GRAYSCALE_16;

        } else if(colorType == PNG_COLOR_TYPE_GRAY_ALPHA){
            format = ImageType::GRAYSCALE_ALPHA_16;

        } else if(colorType == PNG_COLOR_TYPE_RGB){
            format = ImageType::RGB_161616;

        } else if(colorType == PNG_COLOR_TYPE_RGB_ALPHA){
            format = ImageType::RGB_ALPHA_16161616;

        } else {
            fclose(input);
            throw ImageException::invalidFormat();
        }

    } else {
        fclose(input);
        throw ImageException::invalidFormat();
    }

    png_read_update_info(pngPtr, infoPtr);

    row = 0;

    loaded = true;
}

///=============================================================================
void ffw::PngLoader::close(){
    if(pngPtr != NULL){
        png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
    }
    if(input != NULL){
        fclose(input);
    }
    input = NULL;
    pngPtr = NULL;
    infoPtr = NULL;

    width = 0;
    height = 0;
    loaded = 0;
    depth = 0;
    mipmaps = 0;
    mipmapOffset = 0;
    row = 0;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::PngLoader::readRow(void* dest){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(dest == NULL)return 0;
    
    png_read_row(pngPtr, (png_bytep)dest, NULL);
    row++;

    return this->getStrideSize();
}