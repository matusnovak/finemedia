/*** This file is part of FineFramework project ***/

#ifndef PNG_LINKAGE_API
#define PNG_LINKAGE_API extern
#endif
#include <png.h>
#include "ffw/media/pngsaver.h"
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
ffw::PngSaver::PngSaver(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    pngPtr = NULL;
    infoPtr = NULL;
    output = NULL;
    open(path, w, h, type, quality, mips);
}

///=============================================================================
ffw::PngSaver::PngSaver(PngSaver&& other){
    swap(other);
}

///=============================================================================
void ffw::PngSaver::swap(PngSaver& other){
    std::swap(pngPtr, other.pngPtr);
    std::swap(infoPtr, other.infoPtr);
    std::swap(output, other.output);
    std::swap(errorStr, other.errorStr);
}

///=============================================================================
ffw::PngSaver& ffw::PngSaver::operator = (PngSaver&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::PngSaver::~PngSaver(){
    close();
}

///=============================================================================
void ffw::PngSaver::open(const std::string& path, int w, int h, ffw::ImageType type, int quality, int mips){
    (void)mips;
    (void)quality;

    if(w <= 0 || h <= 0)throw ImageException::invalidWidthHeight();

    switch(type){
        case ImageType::GRAYSCALE_8:
        case ImageType::GRAYSCALE_16:
        case ImageType::GRAYSCALE_ALPHA_8:
        case ImageType::GRAYSCALE_ALPHA_16:
        case ImageType::RGB_888:
        case ImageType::RGB_161616:
        case ImageType::RGB_ALPHA_8888:
        case ImageType::RGB_ALPHA_16161616:
            break;
        default:
            throw ImageException::unsupportedImageType();
    }

    output = fopen(path.c_str(), "wb");

    if(!output){
        throw ImageException::invalidFile();
    }

    pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &this->errorStr, user_error_fn, user_warning_fn);
    if (!pngPtr){
        fclose(output);
        throw ImageException(errorStr.empty() ? "png_create_write_struct error" : errorStr);
    }

    infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr){
        fclose(output);
        throw ImageException(errorStr.empty() ? "png_create_info_struct error" : errorStr);
    }

    if (setjmp(png_jmpbuf(pngPtr))){
        fclose(output);
        throw ImageException(errorStr.empty() ? "png_jmpbuf error" : errorStr);
    }

    png_init_io(pngPtr, output);

    bool setSwap = false;
    png_byte colorType = 0;

    int bits;

    switch(type){
        case ImageType::GRAYSCALE_8:
            colorType = PNG_COLOR_TYPE_GRAY;
            bits = 8;
            setSwap = false;
            break;
        case ImageType::GRAYSCALE_16:
            colorType = PNG_COLOR_TYPE_GRAY;
            bits = 16;
            setSwap = true;
            break;
        case ImageType::GRAYSCALE_ALPHA_8:
            colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
            bits = 8;
            setSwap = false;
            break;
        case ImageType::GRAYSCALE_ALPHA_16:
            colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
            bits = 16;
            setSwap = true;
            break;
        case ImageType::RGB_888:
            colorType = PNG_COLOR_TYPE_RGB;
            bits = 8;
            setSwap = false;
            break;
        case ImageType::RGB_161616:
            colorType = PNG_COLOR_TYPE_RGB;
            bits = 16;
            setSwap = true;
            break;
        case ImageType::RGB_ALPHA_8888:
            colorType = PNG_COLOR_TYPE_RGB_ALPHA;
            bits = 8;
            setSwap = false;
            break;
        case ImageType::RGB_ALPHA_16161616:
            colorType = PNG_COLOR_TYPE_RGB_ALPHA;
            bits = 16;
            setSwap = true;
            break;
        default:
            throw ImageException::unsupportedImageType();
    }

    png_set_IHDR(
        pngPtr,
        infoPtr,
        w, h,
        bits,
        colorType,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(pngPtr, infoPtr);

    if(setSwap)
        png_set_swap(pngPtr);

    format = type;
    width = w;
    height = h;
    depth = 0;
    mipmaps = 1;

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::PngSaver::close(){
    if(pngPtr != NULL){
        png_destroy_write_struct(&pngPtr, &infoPtr);
    }
    if(output != NULL){
        fclose(output);
    }
    output = NULL;
    pngPtr = NULL;
    infoPtr = NULL;

    width = 0;
    height = 0;
    loaded = 0;
    depth = 0;
    mipmaps = 0;
    row = 0;
    format = ImageType::INVALID;
}

///=============================================================================
size_t ffw::PngSaver::writeRow(const void* src){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(src == NULL)return 0;
    
    png_write_row(pngPtr, (png_bytep)src);
    row++;

    return this->getStrideSize();
}

///=============================================================================
bool ffw::PngSaver::writeFooter(){
    if(!loaded)return false;
    if(row != height)return false;

    png_write_end(pngPtr, NULL);

    return true;
}