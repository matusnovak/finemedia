cmake_minimum_required(VERSION 3.1)
project (finemedia)

# Some required properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Select build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_INSTALL "Install library" ON)

# Add third party libraries
include(${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs.cmake)

# Grab the files
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/ffw/media/*.h)

# Create the library
add_definitions(-DFFW_EXPORTS)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "fineframework/libs") 
add_dependencies(${PROJECT_NAME} ZLIB)
add_dependencies(${PROJECT_NAME} LIBPNG)
add_dependencies(${PROJECT_NAME} LIBTIFF)
add_dependencies(${PROJECT_NAME} LIBJPEG)
if(UNIX OR APPLE)
  add_dependencies(${PROJECT_NAME} BZIP2)
endif()

# Dependencies
include_directories(${CMAKE_SOURCE_DIR}/include)
if(MSVC)
  link_directories(${CMAKE_BINARY_DIR}/Debug)
  link_directories(${CMAKE_BINARY_DIR}/Release)
endif(MSVC)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} ${LIBPNG_LIBRARY} ${LIBJPEG_LIBRARY} ${LIBTIFF_LIBRARY} ${ZLIB_LIBRARY})
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} ${LIBPNG_LIBRARY} ${LIBJPEG_LIBRARY} ${LIBTIFF_LIBRARY} ${ZLIB_LIBRARY} lzma jbig)
elseif(MSVC)
  target_link_libraries(${PROJECT_NAME} ${LIBPNG_LIBRARY} ${LIBJPEG_LIBRARY} ${LIBTIFF_LIBRARY} ${ZLIB_LIBRARY})
else()
  target_link_libraries(${PROJECT_NAME} ${LIBPNG_LIBRARY} ${LIBJPEG_LIBRARY} ${LIBTIFF_LIBRARY} ${ZLIB_LIBRARY})
endif()

# Additional C++ headers
include_directories(${PROJECT_NAME} PUBLIC ${LIBPNG_INCLUDE_DIR} ${LIBJPEG_INCLUDE_DIR} ${LIBTIFF_INCLUDE_DIR})

# Compile with static stdlib ?
if(STATIC_STDLIB)
    target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif(STATIC_STDLIB)

# Examples extra files
set(EXAMPLES_FILES owl.png)

# Add examples
if(BUILD_EXAMPLES)
    foreach(FILENAME ${EXAMPLES_FILES})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/${FILENAME} DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/${FILENAME} DESTINATION ${CMAKE_BINARY_DIR}/examples)
    endforeach(FILENAME)

    file(GLOB EXAMPLES ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp)
    foreach(FILENAME ${EXAMPLES})
        # Get file name without extension
        get_filename_component(FILENAME_NO_EXT ${FILENAME} NAME_WE)
        set(EXAMPLE_PROJ example_${FILENAME_NO_EXT})
        add_executable(${EXAMPLE_PROJ} ${FILENAME})
        add_dependencies(${EXAMPLE_PROJ} ${PROJECT_NAME})

        # Examples dependencies
        target_link_libraries(${EXAMPLE_PROJ} finemedia)
        if(MSVC)
            set_target_properties(${EXAMPLE_PROJ} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
        endif()

        # Compile with static stdlib ?
        if(STATIC_STDLIB)
            target_link_libraries(${EXAMPLE_PROJ} -static-libgcc -static-libstdc++)
        endif(STATIC_STDLIB)

        set_property(TARGET ${EXAMPLE_PROJ} PROPERTY FOLDER "fineframework/examples") 
    endforeach(FILENAME)
endif()

# Examples extra files
set(TESTS_FILES imgs)

# Add tests
if(BUILD_TESTS)
    enable_testing ()

    foreach(FILENAME ${TESTS_FILES})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/${FILENAME} DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/${FILENAME} DESTINATION ${CMAKE_BINARY_DIR}/tests)
    endforeach(FILENAME)

    file(GLOB TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
    foreach(FILENAME ${TESTS})
        # Get file name without extension
        get_filename_component(FILENAME_NO_EXT ${FILENAME} NAME_WE)
        set(TEST_PROJ test_${FILENAME_NO_EXT})
        add_executable(${TEST_PROJ} ${FILENAME})
        add_dependencies(${TEST_PROJ} ${PROJECT_NAME})

        # Tests dependencies
        target_link_libraries(${TEST_PROJ} finemedia)

        if(MSVC)
            set_target_properties(${TEST_PROJ} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
        endif()

        # Compile with static stdlib ?
        if(STATIC_STDLIB)
            target_link_libraries(${TEST_PROJ} -static-libgcc -static-libstdc++)
        endif(STATIC_STDLIB)

        set_property(TARGET ${TEST_PROJ} PROPERTY FOLDER "fineframework/tests") 
        add_test(NAME ${TEST_PROJ} COMMAND ${TEST_PROJ})
    endforeach(FILENAME)
endif()

# Add install step
if(BUILD_INSTALL)
  install(DIRECTORY include/ffw DESTINATION include)

  if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL)
  endif()
  install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT library)
endif()
