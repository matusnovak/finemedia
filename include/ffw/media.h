/**
 * @defgroup media media - Image file readers and writers
 * @brief Provides classes for reading and writing image files
 * @details Uses the following third party libraries: 
 * [libjpeg](http://www.ijg.org/), 
 * [libpng](http://www.libpng.org/pub/png/libpng.html), 
 * [libtiff](http://www.libtiff.org/), 
 * [libz](http://www.zlib.net/),
 * and [bzip](https://github.com/enthought/bzip2-1.0.6)
 */
#ifndef FFW_MEDIA_MODULE
#define FFW_MEDIA_MODULE
#include "media/ddsloader.h"
#include "media/ddssaver.h"
#include "media/pngloader.h"
#include "media/pngsaver.h"
#include "media/jpgloader.h"
#include "media/jpgsaver.h"
#include "media/bmploader.h"
#include "media/bmpsaver.h"
#include "media/pbmloader.h"
#include "media/pbmsaver.h"
#include "media/tgaloader.h"
#include "media/tgasaver.h"
#include "media/tifloader.h"
#include "media/tifsaver.h"
#include "media/imageutils.h"
#include "media/exceptions.h"
#endif
