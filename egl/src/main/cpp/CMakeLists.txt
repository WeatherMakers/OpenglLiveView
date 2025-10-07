# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(OpenglLiveView)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# 使用 C++20 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()


include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/thirdparty
                    ${NATIVERENDER_ROOT_PATH}/thirdparty/glm
                    ${NATIVERENDER_ROOT_PATH}/thirdparty/stb
                    ${NATIVERENDER_ROOT_PATH}/util
                    )

# ===========  第三方库 JSONCPP（源码方式）  ===========
set(JSONCPP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/jsoncpp)
if(EXISTS ${JSONCPP_SRC_DIR}/CMakeLists.txt)
    set(JSONCPP_WITH_TESTS OFF CACHE INTERNAL "" FORCE)
    set(JSONCPP_WITH_EXAMPLE OFF CACHE INTERNAL "" FORCE)
    set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${JSONCPP_SRC_DIR} thirdparty/jsoncpp)
else()
    message(FATAL_ERROR "jsoncpp source not found at: ${JSONCPP_SRC_DIR}. Please place jsoncpp sources with CMakeLists.txt there.")
endif()
# ===========  第三方库 JSONCPP 配置结束  ===========

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

add_library(egl SHARED ${src-files})

target_compile_features(egl PUBLIC cxx_std_17)

target_link_libraries(egl PUBLIC
    ${hilog-lib}
    ${EGL-lib}
    ${GLES-lib}
    ${libace-lib}
    librawfile.z.so
    libace_napi.z.so
    libc++.a
    jsoncpp
    ${OpenCV_LIBS}
    )