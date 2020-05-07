# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.


if (${ANDROID_PLATFORM_LEVEL} LESS 24)
    message(FATAL_ERROR "OpenGL es 3.2 is not supported before API level 24 \
                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()
endif (${ANDROID_PLATFORM_LEVEL} LESS 24)

# Import the CMakeLists.txt for the glm library
add_subdirectory(glm)

add_library( # Sets the name of the library.
        fire-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        fire/fire.cpp
        fire/rendering/renderer.cpp
        fire/rendering/ray_renderer.cpp
        fire/simulation/simulator.cpp
        fire/simulation/simulation_operations.cpp
        fire/simulation/wavelet_turbulence.cpp
        fire/simulation/slab_operation.cpp
        fire/util/helper.cpp
        fire/util/file_loader.cpp
        fire/util/shader.cpp
        fire/util/simple_framebuffer.cpp
        fire/util/framebuffer.cpp
        fire/util/data_texture_pair.cpp
        fire/util/perlin.cpp
        fire/util/turbulence.cpp
        fire/util/wavelet.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        fire-lib
        android
        log
        EGL
        GLESv3
        glm
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})