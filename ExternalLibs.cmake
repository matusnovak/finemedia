cmake_minimum_required(VERSION 3.1)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

ExternalProject_Add(ZLIB
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/zlib
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
  BUILD_COMMAND cmake --build . --target zlibstatic --config MinSizeRel
  INSTALL_COMMAND ""
  TEST_COMMAND ""
)

if(MSVC)
  set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/zlib)
  set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/MinSizeRel/zlibstatic.lib)
elseif(MINGW)
  set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/zlib)
  set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/libzlibstatic.a)
else()
  set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/zlib)
  set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/libz.a)
endif()

if(MSVC)
  ExternalProject_Add(LIBPNG
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/libpng
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DPNG_FRAMEWORK=OFF -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/libs/zlib -DCMAKE_INCLUDE_PATH=${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build -DZLIB_LIBRARY=${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/MinSizeRel/zlibstatic
    BUILD_COMMAND cmake -E copy ${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/zconf.h ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build && cmake --build . --config MinSizeRel
    INSTALL_COMMAND ""
    TEST_COMMAND ""
  )
  add_dependencies(LIBPNG ZLIB)
elseif(MINGW)
  ExternalProject_Add(LIBPNG
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/libpng
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DPNG_FRAMEWORK=OFF -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/libs/zlib -DZLIB_LIBRARY=${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/libzlibstatic.a
    BUILD_COMMAND cmake -E copy ${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/zconf.h ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build && cmake --build . --config MinSizeRel
    INSTALL_COMMAND ""
    TEST_COMMAND ""
  )
  add_dependencies(LIBPNG ZLIB)
else()
  ExternalProject_Add(LIBPNG
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/libpng
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DPNG_FRAMEWORK=OFF -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/libs/zlib -DZLIB_LIBRARY=${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/libz.a -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    BUILD_COMMAND cmake -E copy ${CMAKE_BINARY_DIR}/ZLIB-prefix/src/ZLIB-build/zconf.h ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build && cmake --build . --config MinSizeRel
    INSTALL_COMMAND ""
    TEST_COMMAND ""
  )
  add_dependencies(LIBPNG ZLIB)
endif()

if(MSVC)
  set(LIBPNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libpng ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build)
  set(LIBPNG_LIBRARY ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build/MinSizeRel/libpng16_static.lib)
else(MSVC)
  set(LIBPNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libpng ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build)
  set(LIBPNG_LIBRARY ${CMAKE_BINARY_DIR}/LIBPNG-prefix/src/LIBPNG-build/libpng16.a)
endif(MSVC)

ExternalProject_Add(LIBTIFF
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/libtiff
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true -Djpeg:BOOL=false -Djbig:BOOL=false -Djpeg12:BOOL=false
  BUILD_COMMAND cmake --build . --target tiff --config MinSizeRel && cmake --build . --target tiff --config Debug
  INSTALL_COMMAND ""
  TEST_COMMAND ""
)

if(MSVC)
  set(LIBTIFF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libtiff/libtiff ${CMAKE_BINARY_DIR}/LIBTIFF-prefix/src/LIBTIFF-build/libtiff)
  set(LIBTIFF_LIBRARY optimized ${CMAKE_BINARY_DIR}/LIBTIFF-prefix/src/LIBTIFF-build/libtiff/MinSizeRel/tiff.lib debug ${CMAKE_BINARY_DIR}/LIBTIFF-prefix/src/LIBTIFF-build/libtiff/Debug/tiffd.lib)
else(MSVC)
  set(LIBTIFF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libtiff/libtiff ${CMAKE_BINARY_DIR}/LIBTIFF-prefix/src/LIBTIFF-build/libtiff)
  set(LIBTIFF_LIBRARY ${CMAKE_BINARY_DIR}/LIBTIFF-prefix/src/LIBTIFF-build/libtiff/libtiff.a)
endif(MSVC)

ExternalProject_Add(LIBJPEG
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/libjpeg
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC=ON -DBUILD_TESTS=OFF -DBUILD_EXECUTABLES=OFF -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
  BUILD_COMMAND cmake --build . --config MinSizeRel
  INSTALL_COMMAND ""
  TEST_COMMAND ""
)

if(MSVC)
  set(LIBJPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libjpeg ${CMAKE_BINARY_DIR}/LIBJPEG-prefix/src/LIBJPEG-build)
  set(LIBJPEG_LIBRARY ${CMAKE_BINARY_DIR}/LIBJPEG-prefix/src/LIBJPEG-build/MinSizeRel/jpeg.lib)
else(MSVC)
  set(LIBJPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libjpeg ${CMAKE_BINARY_DIR}/LIBJPEG-prefix/src/LIBJPEG-build)
  set(LIBJPEG_LIBRARY ${CMAKE_BINARY_DIR}/LIBJPEG-prefix/src/LIBJPEG-build/libjpeg.a)
endif(MSVC)

if(UNIX OR APPLE)
  ExternalProject_Add(BZIP2
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/bzip2-1.0.6
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cd ${CMAKE_SOURCE_DIR}/libs && chmod +x ./make_bzip2.sh && ./make_bzip2.sh ${CMAKE_BINARY_DIR}/BZIP2-prefix/src/BZIP2-build
    INSTALL_COMMAND ""
    TEST_COMMAND ""
  )

  set(BZIP2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/bzip2-1.0.6)
  set(BZIP2_LIBRARY ${CMAKE_BINARY_DIR}/BZIP2-prefix/src/BZIP2-build/libbz2.a)
endif()
