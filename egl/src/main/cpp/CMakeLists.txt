# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(OpenglLiveView)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

# 查找相关库 (包括OpenGL ES相关库和XComponent提供的ndk接口)
find_library(EGL-lib EGL)
find_library(GLES-lib GLESv3)
find_library(libace-lib ace_ndk.z)
find_library(hilog-lib hilog_ndk.z)

file(GLOB src-files
    *.cpp
    render/*.cpp
    util/*.cpp
    render/*.cpp
    example/*.cpp)

add_library(egl SHARED
    ${src-files})
target_link_libraries(egl PUBLIC
    ${hilog-lib}
    ${EGL-lib}
    ${GLES-lib}
    ${libace-lib}
    libace_napi.z.so
    libc++.a)
