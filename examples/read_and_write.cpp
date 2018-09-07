#include <ffw/media.h>
#include <iostream>

int main(int argc, char *argv[]) {
    const auto filename = "owl.png";
    std::unique_ptr<unsigned char[]> pixels;
    std::unique_ptr<unsigned char[]> scaled;
    ffw::ImageType outType;
    int outWidth;
    int outHeight;
    int outStrideSize;
    int outChannels;

    try {
        const auto reader = ffw::openImageReader(filename);

        std::cout << "> Image: " << filename << std::endl;
        std::cout << "size: " << reader->getWidth() << "x" << reader->getHeight() << std::endl;
        std::cout << "channels: " << reader->getNumberOfChannels() << std::endl;
        std::cout << "pixel bit depth: " << reader->getBitDepth() << std::endl;
        std::cout << "bits per pixel: " << reader->getBitsPerPixel() << std::endl;
        std::cout << "number of bytes per row: " << reader->getStrideSize() << std::endl;
        std::cout << std::endl;

        if (reader->getBitDepth() != 8) {
            std::cerr << "Sorry, this example contains a simple ugly algorithm to scale an image!" << std::endl;
            std::cerr << "And therefore only images of bith depth of 8 are supported!" << std::endl;
            return EXIT_FAILURE;
        }

        // We will allocate an array of pixels that will be used to read the image
        pixels.reset(new unsigned char[reader->getAllocationSize()]);

        // Read entire image
        reader->readAll(pixels.get());

        // We will also allocate a second array which will be used to rescale the read image.
        // Since we are scaling down the image 2x, we will need half the size of the row and half
        // the height of the image.

        // this is essentially width * channels * bitdepth_in_bytes
        outStrideSize = reader->getStrideSize() / 2;

        outType = reader->getImageType();
        outWidth = reader->getWidth() / 2;
        outHeight = reader->getHeight() / 2;
        outChannels = reader->getNumberOfChannels();
        scaled.reset(new unsigned char[outStrideSize * outHeight]);

    } 
    catch (ffw::ImageException& e) {
        std::cerr 
            << "Something went wrong while loading " << filename 
            << " error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Scale down the image.
    // Very Very slow algorithm. We are doing this only for the purpose of this example!
    const auto src = pixels.get();
    const auto dst = scaled.get();
    for (int y = 0; y < outHeight; y++) {
        for (int x = 0; x < outWidth; x++) {
            // We will use nearest neighbour scaling algorithm
            for (int c = 0; c < outChannels; c++) {
                const auto pixel = src[(y * 2) * (outStrideSize * 2) + (x * 2) * outChannels + c];
                
                dst[y * outStrideSize + x * outChannels + c] = pixel;
            }
        }
    }

    try {
        const auto writer = ffw::openImageWriter("scaled.png", outWidth, outHeight, outType);

        writer->writeAll(scaled.get());
        writer->writeFooter();

        std::cout << "Scaled down image scaved into: scaled.png" << std::endl;
    }
    catch (ffw::ImageException& e) {
        std::cerr
            << "Something went wrong while writing scaled.png error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
