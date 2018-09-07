/*** This file is part of FineFramework project ***/

#include <memory>
#include "ffw/media/imageutils.h"
#include "ffw/media/pngloader.h"
#include "ffw/media/pngsaver.h"
#include "ffw/media/bmploader.h"
#include "ffw/media/bmpsaver.h"
#include "ffw/media/jpgloader.h"
#include "ffw/media/jpgsaver.h"
#include "ffw/media/pbmloader.h"
#include "ffw/media/pbmsaver.h"
#include "ffw/media/tgaloader.h"
#include "ffw/media/tgasaver.h"
#include "ffw/media/tifloader.h"
#include "ffw/media/tifsaver.h"
#include "ffw/media/ddsloader.h"
#include "ffw/media/ddssaver.h"
#include "ffw/media/exceptions.h"

static std::string toLowerStr(std::string path) {
    for (auto& c : path)c = tolower(c);
    return path;
}

static std::string extension(const std::string& path) {
    const auto pos = path.find_last_of('.');
    if (pos == std::string::npos) return "";
    else return path.substr(pos + 1);
}

///=============================================================================
ffw::ImageReader* ffw::openImageReader(const std::string& path){    
    std::string ext = ::toLowerStr(::extension(path));
    if     (ext == "png")return new ffw::PngLoader(path);
    else if(ext == "jpg")return new ffw::JpgLoader(path);
    else if(ext == "bmp")return new ffw::BmpLoader(path);
    else if(ext == "tga")return new ffw::TgaLoader(path);
    else if(ext == "pbm")return new ffw::PbmLoader(path);
    else if(ext == "tif")return new ffw::TifLoader(path);
    else if(ext == "tiff")return new ffw::TifLoader(path);
    else if(ext == "dds")return new ffw::DdsLoader(path);
    else throw ImageException("unknown image file extension");
}

///=============================================================================
ffw::ImageWriter* ffw::openImageWriter(const std::string& path, int width, int height, ffw::ImageType type, int quality, int mips){
    ffw::ImageWriter* ret = NULL;
    
    std::string ext = ::toLowerStr(::extension(path));
    if     (ext == "png")return new ffw::PngSaver(path, width, height, type, quality, mips);
    else if(ext == "jpg")return new ffw::JpgSaver(path, width, height, type, quality, mips);
    else if(ext == "bmp")return new ffw::BmpSaver(path, width, height, type, quality, mips);
    else if(ext == "tga")return new ffw::TgaSaver(path, width, height, type, quality, mips);
    else if(ext == "pbm")return new ffw::PbmSaver(path, width, height, type, quality, mips);
    else if(ext == "tif")return new ffw::TifSaver(path, width, height, type, quality, mips);
    else if(ext == "tiff")return new ffw::TifSaver(path, width, height, type, quality, mips);
    else if(ext == "dds")return new ffw::DdsSaver(path, width, height, type, quality, mips);
    else throw ImageException("unknown image file extension");
}

///=============================================================================
void ffw::readImage(const std::string& path, void** dest, int* width, int* height, ffw::ImageType* format, int* mips){
    std::unique_ptr<ImageReader> loader(openImageReader(path));

    bool res = true;
    if (dest) {
        *dest = new unsigned char[loader->getAllocationSize()];

        res = loader->readAll(*dest);
        if (!res) {
            delete[](unsigned char*)*dest;
            *dest = nullptr;
        }
    }

    if (width)*width = loader->getWidth();
    if (height)*height = loader->getHeight();
    if (format)*format = loader->getImageType();
    if (mips)*mips = loader->getNumOfMipMaps();
}

///=============================================================================
void ffw::writeImage(const std::string& path, const void* src, int width, int height, ffw::ImageType format, int quality, int mips){
    if (src == nullptr)throw ImageException("source pixels is null");

    std::unique_ptr<ImageWriter> saver(openImageWriter(path, width, height, format, quality, mips));

    saver->writeAll(reinterpret_cast<const unsigned char*>(src));

    saver->writeFooter();
}

///=============================================================================
void ffw::readImage(const std::string& path, ffw::ImageBuffer& image){
    std::unique_ptr<ImageReader> loader(openImageReader(path));

    if (!loader->readAll(image))throw ImageException("failed to read pixels");
}

///=============================================================================
void ffw::writeImage(const std::string& path, const ffw::ImageBuffer& image, int quality){
    if(!image.isAllocated())throw ImageException("source buffer is empty");

    std::unique_ptr<ImageWriter> saver(openImageWriter(path, image.getWidth(), image.getHeight(), image.getImageType(), quality, image.getNumOfMipMaps()));

    if (!saver->writeAll(image))throw ImageException("failed to write pixels");

    saver->writeFooter();
}