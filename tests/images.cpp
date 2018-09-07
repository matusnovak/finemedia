#include <ffw/media.h>
#include <fstream>
#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

static size_t getFileSize(std::fstream& stream){
    stream.seekg(0, std::ios::end);   
    size_t size = (size_t)stream.tellg();
    stream.seekg(0, std::ios::beg);
    return size;
}

static float comparePixels(unsigned char* first, unsigned char* second, size_t size){
    size_t cnt = 0;
    for(size_t i = 0; i < size; i++){
        if(std::abs(first[i] - second[i]) < 255*0.05)cnt++;
    }
    return float(cnt) / float(size);
}

template<class Loader, class Saver>
static void TestImage(
    const std::string& name,
    const std::string& ext,
    int bitDepth,
    int bitsPerPixels,
    ffw::ImageType format,
    int width,
    int height,
    int depth,
    int mipmaps,
    bool compressed,
    int stride,
    int numOfChannels,
    float compare = 0.85
    ){

    std::unique_ptr<Loader> input(new Loader("imgs/" + name + "." + ext));
    
    /*REQUIRE(input->isOpen() == false);
    CHECK(input->getBitDepth() == 0);
    CHECK(input->getBitsPerPixel() == 0);
    CHECK(input->getImageType() == ffw::ImageType::INVALID);
    CHECK(input->getHeight() == 0);
    CHECK(input->getWidth() == 0);
    CHECK(input->getDepth() == 0);
    CHECK(input->isCompressed() == false);
    CHECK(input->getNumOfMipMaps() == 0);
    CHECK(input->getStrideSize() == 0);
    CHECK(input->getRowOffset() == 0);
    CHECK(input->getNumberOfChannels() == 0);*/

    //REQUIRE(input->open("imgs/" + name + "." + ext));

    std::cout << "reading image: " << name << "." << ext << " width: " << input->getWidth() << " height: " << input->getHeight() << std::endl;

    REQUIRE(input->isOpen() == true);
    CHECK(input->getImageType() == format);
    CHECK(input->getBitDepth() == bitDepth);
    CHECK(input->getBitsPerPixel() == bitsPerPixels);
    CHECK(input->getHeight() == height);
    CHECK(input->getWidth() == width);
    CHECK(input->getDepth() == depth);
    CHECK(input->isCompressed() == compressed);
    CHECK(input->getNumOfMipMaps() == mipmaps);
    CHECK(input->getStrideSize() == stride);
    CHECK(input->getRowOffset() == 0);
    CHECK(input->getNumberOfChannels() == numOfChannels);
    
    std::fstream raw;
    raw.open("imgs/" + name + ".raw", std::ios::in | std::ios::binary);
    REQUIRE(raw);

    size_t size = getFileSize(raw);

    REQUIRE(size == height * stride);

    std::unique_ptr<unsigned char> rawbytes(new unsigned char[size]);

    // Read RAW image
    raw.read((char*)rawbytes.get(), size);
    raw.close();

    // Read tested image
    ffw::ImageBuffer imgbytes;
    input->readAll(imgbytes);

    // compare pixels
    float factor = comparePixels(rawbytes.get(), imgbytes.getPtr(), size);

    std::fstream out(name + ".tmp." + ext + ".raw", std::ios::out | std::ios::trunc | std::ios::binary);
    out.write((char*)imgbytes.getPtr(), imgbytes.getAllocationSize(imgbytes.getNumOfMipMaps()));
    out.close();

    // Test if two image buffers are at least 95% same
    if(factor < compare){
        std::cerr << "Similarity between " << ("imgs/" + name + ".raw") << " and " << ("imgs/" + name + "." + ext) << " is too low: " << factor << std::endl;
        REQUIRE(false);
    }

    input->close();

    REQUIRE(input->isOpen() == false);
    CHECK(input->getBitDepth() == 0);
    CHECK(input->getBitsPerPixel() == 0);
    CHECK(input->getImageType() == ffw::ImageType::INVALID);
    CHECK(input->getHeight() == 0);
    CHECK(input->getWidth() == 0);
    CHECK(input->getDepth() == 0);
    CHECK(input->isCompressed() == false);
    CHECK(input->getNumOfMipMaps() == 0);
    CHECK(input->getStrideSize() == 0);
    CHECK(input->getRowOffset() == 0);
    CHECK(input->getNumberOfChannels() == 0);

    std::unique_ptr<Saver> output(new Saver(name + ".tmp." + ext, width, height, format, 100));

    /*REQUIRE(output->isOpen() == false);
    CHECK(output->getBitDepth() == 0);
    CHECK(output->getBitsPerPixel() == 0);
    CHECK(output->getImageType() == ffw::ImageType::INVALID);
    CHECK(output->getHeight() == 0);
    CHECK(output->getWidth() == 0);
    CHECK(output->getDepth() == 0);
    CHECK(output->isCompressed() == false);
    CHECK(output->getNumOfMipMaps() == 0);
    CHECK(output->getStrideSize() == 0);
    CHECK(output->getRowOffset() == 0);
    CHECK(output->getNumberOfChannels() == 0);*/

    //REQUIRE(output->open(name + ".tmp." + ext, width, height, format, 100) == true);

    std::cout << "writing image: " << name << "." << ext << " width: " << output->getWidth() << " height: " << output->getHeight() << std::endl;

    REQUIRE(output->isOpen() == true);
    CHECK(output->getImageType() == format);
    CHECK(output->getBitDepth() == bitDepth);
    CHECK(output->getBitsPerPixel() == bitsPerPixels);
    CHECK(output->getHeight() == height);
    CHECK(output->getWidth() == width);
    CHECK(output->getDepth() == depth);
    CHECK(output->isCompressed() == compressed);
    CHECK(output->getNumOfMipMaps() == 1);
    CHECK(output->getStrideSize() == stride);
    CHECK(output->getRowOffset() == 0);
    CHECK(output->getNumberOfChannels() == numOfChannels);

    // Save all pixels
    output->writeAll(rawbytes.get());
    
    output->close();

    REQUIRE(output->isOpen() == false);
    CHECK(output->getBitDepth() == 0);
    CHECK(output->getBitsPerPixel() == 0);
    CHECK(output->getImageType() == ffw::ImageType::INVALID);
    CHECK(output->getHeight() == 0);
    CHECK(output->getWidth() == 0);
    CHECK(output->getDepth() == 0);
    CHECK(output->isCompressed() == false);
    CHECK(output->getNumOfMipMaps() == 0);
    CHECK(output->getStrideSize() == 0);
    CHECK(output->getRowOffset() == 0);
    CHECK(output->getNumberOfChannels() == 0);

    // Load all pixels back and compare them once more
    std::unique_ptr<Loader> tester(new Loader(name + ".tmp." + ext));

    REQUIRE(tester->isOpen() == true);
    CHECK(tester->getImageType() == format);
    CHECK(tester->getBitDepth() == bitDepth);
    CHECK(tester->getBitsPerPixel() == bitsPerPixels);
    CHECK(tester->getHeight() == height);
    CHECK(tester->getWidth() == width);
    CHECK(tester->getDepth() == depth);
    CHECK(tester->isCompressed() == compressed);
    CHECK(tester->getNumOfMipMaps() == 1);
    CHECK(tester->getStrideSize() == stride);
    CHECK(tester->getRowOffset() == 0);
    CHECK(tester->getNumberOfChannels() == numOfChannels);

    std::cout << "testing image: " << name << "." << ext << " width: " << tester->getWidth() << " height: " << tester->getHeight() << std::endl;

    // Read image
    tester->readAll(imgbytes);

    // compare pixels
    factor = comparePixels(rawbytes.get(), imgbytes.getPtr(), size);

    // Test if two image buffers are at least 95% same
    if(factor < compare){
        std::cerr << "Similarity between " << ("imgs/" + name + ".raw") << " and temporary saved image is too low: " << factor << std::endl;
        REQUIRE(false);
    }

    tester->close();
}

TEST_CASE("PNG_GRAYSCALE_8", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("GRAYSCALE_8", "png", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 1, false, 181 * 1, 1);
}

TEST_CASE("PNG_GRAYSCALE_16", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("GRAYSCALE_16", "png", 16, 16, ffw::ImageType::GRAYSCALE_16, 181, 141, 0, 1, false, 181 * 2, 1);
}

TEST_CASE("PNG_GRAYSCALE_ALPHA_8", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("GRAYSCALE_ALPHA_8", "png", 8, 16, ffw::ImageType::GRAYSCALE_ALPHA_8, 181, 141, 0, 1, false, 181 * 2, 2);
}

TEST_CASE("PNG_GRAYSCALE_ALPHA_16", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("GRAYSCALE_ALPHA_16", "png", 16, 32, ffw::ImageType::GRAYSCALE_ALPHA_16, 181, 141, 0, 1, false, 181 * 4, 2);
}

TEST_CASE("PNG_RGB_888", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("RGB_888", "png", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 1, false, 181 * 3, 3);
}

TEST_CASE("PNG_RGB_161616", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("RGB_161616", "png", 16, 48, ffw::ImageType::RGB_161616, 181, 141, 0, 1, false, 181 * 6, 3);
}

TEST_CASE("PNG_RGB_ALPHA_8888", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("RGB_ALPHA_8888", "png", 8, 32, ffw::ImageType::RGB_ALPHA_8888, 181, 141, 0, 1, false, 181 * 4, 4);
}

TEST_CASE("PNG_RGB_ALPHA_16161616", "[PNG]"){
    TestImage<ffw::PngLoader, ffw::PngSaver>("RGB_ALPHA_16161616", "png", 16, 64, ffw::ImageType::RGB_ALPHA_16161616, 181, 141, 0, 1, false, 181 * 8, 4);
}

TEST_CASE("JPG_GRAYSCALE_8", "[JPG]"){
    TestImage<ffw::JpgLoader, ffw::JpgSaver>("GRAYSCALE_8", "jpg", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 1, false, 181 * 1, 1);
}

TEST_CASE("JPG_RGB_888", "[JPG]"){
    TestImage<ffw::JpgLoader, ffw::JpgSaver>("RGB_888", "jpg", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 1, false, 181 * 3, 3);
}

TEST_CASE("BMP_BITMAP_1", "[BMP]"){
    TestImage<ffw::BmpLoader, ffw::BmpSaver>("BITMAP_1", "bmp", 1, 1, ffw::ImageType::BITMAP_1, 181, 141, 0, 1, false, 23, 1);
}

TEST_CASE("BMP_GRAYSCALE_4", "[BMP]"){
    TestImage<ffw::BmpLoader, ffw::BmpSaver>("GRAYSCALE_4", "bmp", 4, 4, ffw::ImageType::GRAYSCALE_4, 181, 141, 0, 1, false, 91, 1);
}

TEST_CASE("BMP_GRAYSCALE_8", "[BMP]"){
    TestImage<ffw::BmpLoader, ffw::BmpSaver>("GRAYSCALE_8", "bmp", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 1, false, 181 * 1, 1);
}

TEST_CASE("BMP_RGB_888", "[BMP]"){
    TestImage<ffw::BmpLoader, ffw::BmpSaver>("RGB_888", "bmp", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 1, false, 181 * 3, 3);
}

TEST_CASE("BMP_RGB_ALPHA_4444", "[BMP]"){
    TestImage<ffw::BmpLoader, ffw::BmpSaver>("RGB_ALPHA_4444", "bmp", 4, 16, ffw::ImageType::RGB_ALPHA_4444, 181, 141, 0, 1, false, 181 * 2, 4, 0.75);
}

TEST_CASE("BMP_RGB_ALPHA_8888", "[BMP]"){
    TestImage<ffw::BmpLoader, ffw::BmpSaver>("RGB_ALPHA_8888", "bmp", 8, 32, ffw::ImageType::RGB_ALPHA_8888, 181, 141, 0, 1, false, 181 * 4, 4, 0.75);
}

TEST_CASE("PBM_GRAYSCALE_8", "[PBM]"){
    TestImage<ffw::PbmLoader, ffw::PbmSaver>("GRAYSCALE_8", "pbm", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 1, false, 181 * 1, 1);
}

TEST_CASE("PBM_GRAYSCALE_16", "[PBM]"){
    TestImage<ffw::PbmLoader, ffw::PbmSaver>("GRAYSCALE_16", "pbm", 16, 16, ffw::ImageType::GRAYSCALE_16, 181, 141, 0, 1, false, 181 * 2, 1);
}

TEST_CASE("PBM_GRAYSCALE_32F", "[PBM]"){
    TestImage<ffw::PbmLoader, ffw::PbmSaver>("GRAYSCALE_32F", "pbm", 32, 32, ffw::ImageType::GRAYSCALE_32F, 181, 141, 0, 1, false, 181 * 4, 1);
}

TEST_CASE("PBM_RGB_888", "[PBM]"){
    TestImage<ffw::PbmLoader, ffw::PbmSaver>("RGB_888", "pbm", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 1, false, 181 * 3, 3);
}

TEST_CASE("PBM_RGB_161616", "[PBM]"){
    TestImage<ffw::PbmLoader, ffw::PbmSaver>("RGB_161616", "pbm", 16, 48, ffw::ImageType::RGB_161616, 181, 141, 0, 1, false, 181 * 6, 3);
}

TEST_CASE("PBM_RGB_323232F", "[PBM]"){
    TestImage<ffw::PbmLoader, ffw::PbmSaver>("RGB_323232F", "pbm", 32, 96, ffw::ImageType::RGB_323232F, 181, 141, 0, 1, false, 181 * 12, 3);
}

TEST_CASE("TGA_GRAYSCALE_8", "[TGA]"){
    TestImage<ffw::TgaLoader, ffw::TgaSaver>("GRAYSCALE_8", "tga", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 1, false, 181 * 1, 1);
}

TEST_CASE("TGA_RGB_ALPHA_5551", "[TGA]"){
    TestImage<ffw::TgaLoader, ffw::TgaSaver>("RGB_ALPHA_5551", "tga", 5, 16, ffw::ImageType::RGB_ALPHA_5551, 181, 141, 0, 1, false, 181 * 2, 4);
}

TEST_CASE("TGA_RGB_888", "[TGA]"){
    TestImage<ffw::TgaLoader, ffw::TgaSaver>("RGB_888", "tga", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 1, false, 181 * 3, 3);
}

TEST_CASE("TGA_RGB_ALPHA_8888", "[TGA]"){
    TestImage<ffw::TgaLoader, ffw::TgaSaver>("RGB_ALPHA_8888", "tga", 8, 32, ffw::ImageType::RGB_ALPHA_8888, 181, 141, 0, 1, false, 181 * 4, 4);
}

TEST_CASE("TIF_GRAYSCALE_8", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("GRAYSCALE_8", "tif", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 1, false, 181 * 1, 1);
}

TEST_CASE("TIF_GRAYSCALE_16", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("GRAYSCALE_16", "tif", 16, 16, ffw::ImageType::GRAYSCALE_16, 181, 141, 0, 1, false, 181 * 2, 1);
}

TEST_CASE("TIF_GRAYSCALE_32F", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("GRAYSCALE_32F", "tif", 32, 32, ffw::ImageType::GRAYSCALE_32F, 181, 141, 0, 1, false, 181 * 4, 1);
}

TEST_CASE("TIF_GRAYSCALE_ALPHA_8", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("GRAYSCALE_ALPHA_8", "tif", 8, 16, ffw::ImageType::GRAYSCALE_ALPHA_8, 181, 141, 0, 1, false, 181 * 2, 2, 0.10f);
}

TEST_CASE("TIF_GRAYSCALE_ALPHA_16", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("GRAYSCALE_ALPHA_16", "tif", 16, 32, ffw::ImageType::GRAYSCALE_ALPHA_16, 181, 141, 0, 1, false, 181 * 4, 2, 0.10f);
}

//TEST_CASE(TIF_GRAYSCALE_ALPHA_32F){
//	TestImage<ffw::TifLoader, ffw::TifSaver>("GRAYSCALE_ALPHA_32F", "tif", 32, 64, ffw::ImageType::GRAYSCALE_ALPHA_32F, 181, 141, 181 * 8, 2);
//}

#if defined(_MSC_VER) && _MSC_VER == 1800 && (defined(_DEBUG) || defined(DEBUG)
// libtiff freezes if run on Debug mode in Visual Studio 2013
// No one knows why
#else
TEST_CASE("TIF_RGB_888", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("RGB_888", "tif", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 1, false, 181 * 3, 3);
}

TEST_CASE("TIF_RGB_161616", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("RGB_161616", "tif", 16, 48, ffw::ImageType::RGB_161616, 181, 141, 0, 1, false, 181 * 6, 3);
}

TEST_CASE("TIF_RGB_323232F", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("RGB_323232F", "tif", 32, 96, ffw::ImageType::RGB_323232F, 181, 141, 0, 1, false, 181 * 12, 3);
}

TEST_CASE("TIF_RGB_ALPHA_8888", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("RGB_ALPHA_8888", "tif", 8, 32, ffw::ImageType::RGB_ALPHA_8888, 181, 141, 0, 1, false, 181 * 4, 4, 0.60f);
}

TEST_CASE("TIF_RGB_ALPHA_16161616", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("RGB_ALPHA_16161616", "tif", 16, 64, ffw::ImageType::RGB_ALPHA_16161616, 181, 141, 0, 1, false, 181 * 8, 4, 0.60f);
}

TEST_CASE("TIF_RGB_ALPHA_32323232F", "[TIF]"){
    TestImage<ffw::TifLoader, ffw::TifSaver>("RGB_ALPHA_32323232F", "tif", 32, 128, ffw::ImageType::RGB_ALPHA_32323232F, 181, 141, 0, 1, false, 181 * 16, 4, 0.60f);
}
#endif

TEST_CASE("DDS_GRAYSCALE_8", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("GRAYSCALE_8", "dds", 8, 8, ffw::ImageType::GRAYSCALE_8, 181, 141, 0, 8, false, 181 * 1, 1);
}

TEST_CASE("DDS_GRAYSCALE_16F", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("GRAYSCALE_16F", "dds", 16, 16, ffw::ImageType::GRAYSCALE_16F, 181, 141, 0, 8, false, 181 * 2, 1);
}

TEST_CASE("DDS_GRAYSCALE_32F", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("GRAYSCALE_32F", "dds", 32, 32, ffw::ImageType::GRAYSCALE_32F, 181, 141, 0, 8, false, 181 * 4, 1);
}

TEST_CASE("DDS_RG_1616", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RG_1616", "dds", 16, 32, ffw::ImageType::RG_1616, 181, 141, 0, 8, false, 181 * 4, 2);
}

TEST_CASE("DDS_RG_1616F", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RG_1616F", "dds", 16, 32, ffw::ImageType::RG_1616F, 181, 141, 0, 8, false, 181 * 4, 2);
}

TEST_CASE("DDS_RGB_888", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_888", "dds", 8, 24, ffw::ImageType::RGB_888, 181, 141, 0, 8, false, 181 * 3, 3);
}

TEST_CASE("DDS_RGB_ALPHA_4444", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_ALPHA_4444", "dds", 4, 16, ffw::ImageType::RGB_ALPHA_4444, 181, 141, 0, 8, false, 181 * 2, 4);
}

TEST_CASE("DDS_RGB_ALPHA_8888", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_ALPHA_8888", "dds", 8, 32, ffw::ImageType::RGB_ALPHA_8888, 181, 141, 0, 8, false, 181 * 4, 4);
}

TEST_CASE("DDS_RGB_ALPHA_16161616F", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_ALPHA_16161616F", "dds", 16, 64, ffw::ImageType::RGB_ALPHA_16161616F, 181, 141, 0, 8, false, 181 * 4 * 2, 4);
}

TEST_CASE("DDS_RGB_ALPHA_32323232F", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_ALPHA_32323232F", "dds", 32, 128, ffw::ImageType::RGB_ALPHA_32323232F, 181, 141, 0, 8, false, 181 * 4 * 4, 4);
}

TEST_CASE("DDS_RGB_DXT1", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_DXT1", "dds", 8, 4, ffw::ImageType::RGB_DXT1, 256, 256, 0, 9, true, 256 * 0.5, 3);
}

TEST_CASE("DDS_RGB_ALPHA_DXT3", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_ALPHA_DXT3", "dds", 8, 8, ffw::ImageType::RGB_ALPHA_DXT3, 256, 256, 0, 9, true, 256, 4);
}

TEST_CASE("DDS_RGB_ALPHA_DXT5", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RGB_ALPHA_DXT5", "dds", 8, 8, ffw::ImageType::RGB_ALPHA_DXT5, 256, 256, 0, 9, true, 256, 4);
}

TEST_CASE("DDS_RED_RGTC1", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RED_RGTC1", "dds", 8, 4, ffw::ImageType::RED_RGTC1, 256, 256, 0, 9, true, 256 * 0.5, 1);
}

TEST_CASE("DDS_RED_GREEN_RGTC2", "[DDS]") {
    TestImage<ffw::DdsLoader, ffw::DdsSaver>("RED_GREEN_RGTC2", "dds", 8, 8, ffw::ImageType::RED_GREEN_RGTC2, 256, 256, 0, 9, true, 256, 2);
}
