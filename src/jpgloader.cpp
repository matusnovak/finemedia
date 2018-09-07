/*** This file is part of FineFramework project ***/

#include "ffw/media/jpgloader.h"
#include <jpeglib.h>
#include <setjmp.h>
#include "ffw/media/exceptions.h"

///=============================================================================
METHODDEF(void) my_error_exit (j_common_ptr cinfo){
    // cinfo->err really points to a my_error_mgr struct, so coerce pointer
    //my_error_ptr myerr = (my_error_ptr) cinfo->err;

    // Always display the message.
    // We could postpone this until after returning, if we chose.
    (*cinfo->err->output_message) (cinfo);

    // Return control to the setjmp point
    //try {
        //longjmp(myerr->setjmp_buffer, 1);
    //} catch (const std::exception& e){
    //    std::cout << "JPG internal error: " << e.what() << std::endl;
    //}
}

///=============================================================================
struct ffw::JpgLoader::jpg_struct_s {
    struct my_error_mgr {
        // "public" fields
        struct jpeg_error_mgr pub;
        // for return to caller
        jmp_buf setjmp_buffer;
    };

    typedef struct my_error_mgr * my_error_ptr;

    jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
};

///=============================================================================
ffw::JpgLoader::JpgLoader(const std::string& path):jpg_struct(new jpg_struct_s){
    input = NULL;
    cInfoInit = false;
    decompressInit = false;
    open(path);
}

///=============================================================================
ffw::JpgLoader::JpgLoader(JpgLoader&& other){
    swap(other);
}

///=============================================================================
void ffw::JpgLoader::swap(JpgLoader& other){
    std::swap(jpg_struct, other.jpg_struct);
    std::swap(cInfoInit, other.cInfoInit);
    std::swap(decompressInit, other.decompressInit);
    std::swap(input, other.input);
}

///=============================================================================
ffw::JpgLoader& ffw::JpgLoader::operator = (JpgLoader&& other){
    if(this != &other){
        swap(other);
    }
    return *this;
}

///=============================================================================
ffw::JpgLoader::~JpgLoader(){
    close();
    if(jpg_struct != NULL){
        delete jpg_struct;
    }
}

///=============================================================================
void ffw::JpgLoader::open(const std::string& path){
    input = fopen(path.c_str(), "rb");

    if(!input){
        throw ImageException::invalidFile();
    }

    // Step 1: allocate and initialize JPEG decompression object

    // We set up the normal JPEG error routines, then override error_exit.
    jpg_struct->cinfo.err = jpeg_std_error(&jpg_struct->jerr.pub);
    jpg_struct->jerr.pub.error_exit = my_error_exit;
    // Establish the setjmp return context for my_error_exit to use.
    if(setjmp(jpg_struct->jerr.setjmp_buffer)) {
        // If we get here, the JPEG code has signaled an error.
        // We need to clean up the JPEG object, close the input file, and return.
        close();
        throw ImageException("Failed to set jpeg setjmp");
    }

    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&jpg_struct->cinfo);
    cInfoInit = true;

    jpg_struct->cinfo.client_data = this;

    // Step 2: specify data source (eg, a file)

    jpeg_stdio_src(&jpg_struct->cinfo, input);

    // Step 3: read file parameters with jpeg_read_header()

    jpeg_read_header(&jpg_struct->cinfo, TRUE);
    // We can ignore the return value from jpeg_read_header since
    // (a) suspension is not possible with the stdio data source, and
    // (b) we passed TRUE to reject a tables-only JPEG file as an error.
    // See libjpeg.txt for more info.

    // Step 5: Start decompressor

    jpeg_start_decompress(&jpg_struct->cinfo);
    decompressInit = true;
    // We can ignore the return value since suspension is not possible
    // with the stdio data source.

    // We may need to do some setup of our own at this point before reading
    // the data.  After jpeg_start_decompress() we have the correct scaled
    // output image dimensions available, as well as the output colormap
    // if we asked for color quantization.
    // In this example, we need to make an output work buffer of the right size.

    if(jpg_struct->cinfo.output_components == 1){
        format = ffw::ImageType::GRAYSCALE_8;
    } else if(jpg_struct->cinfo.output_components == 3){
        format = ffw::ImageType::RGB_888;
    } else {
        close();
        throw ImageException::invalidFormat();
    }

    width = jpg_struct->cinfo.output_width;
    height = jpg_struct->cinfo.output_height;
    depth = 0;
    mipmaps = 1;

    row = 0;
    loaded = true;
}

///=============================================================================
void ffw::JpgLoader::close(){
    if(jpg_struct != NULL){
        if(decompressInit){
            jpeg_finish_decompress(&jpg_struct->cinfo);
        }
        if(cInfoInit){
            jpeg_destroy_decompress(&jpg_struct->cinfo);
        }
    }
    if(input != NULL){
        fclose(input);
    }
    decompressInit = false;
    cInfoInit = false;
    input = NULL;
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
size_t ffw::JpgLoader::readRow(void* dest){
    if(!loaded)return 0;
    if(row >= height)return 0;
    if(dest == NULL)return 0;

    JSAMPROW row_pointer[1];
    row_pointer[0] = (decltype(row_pointer[0]))dest;
    jpeg_read_scanlines(&jpg_struct->cinfo, row_pointer, 1);
    row++;

    return this->getStrideSize();
}