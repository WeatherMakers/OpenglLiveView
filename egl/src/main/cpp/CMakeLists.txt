# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(OpenglLiveView)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

# ===========  第三方库 KTX 路径设置  ===========  KTX 目前使用的架构是基于Android的so文件，不能够链接到鸿蒙中
#if(OHOS_ARCH STREQUAL "arm64-v8a")
#    message(STATUS "Configuring for HarmonyOS arm64-v8a ABI")
#    set(KTX_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ktx/arm64-v8a/lib)
#    set(KTX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ktx/arm64-v8a/include)
#elseif(OHOS_ARCH STREQUAL "x86_64")
#    message(STATUS "Configuring for HarmonyOS x86_64 ABI (Simulator)")
#    set(KTX_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ktx/x86_64/lib)
#    set(KTX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ktx/x86_64/include)
#else()
#    message(FATAL_ERROR "Unsupported ABI: ${OHOS_ARCH}")
#endif()
#
## 2. 将 KTX 的头文件目录添加到工程中
#include_directories(${KTX_INCLUDE_PATH})
#
## 3. 添加预编译的 libktx.so 作为一个 IMPORTED 库
#add_library(ktx SHARED IMPORTED)
#set_target_properties(ktx PROPERTIES IMPORTED_LOCATION ${KTX_LIB_PATH}/libktx.so)
# ===========  第三方库 KTX 配置结束  ===========

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

# 确保目标也声明使用 C++17 特性
target_compile_features(egl PUBLIC cxx_std_17)

target_link_libraries(egl PUBLIC
    ${hilog-lib}
    ${EGL-lib}
    ${GLES-lib}
    ${libace-lib}
    librawfile.z.so
    libace_napi.z.so
    libc++.a
#    ktx
    jsoncpp
    )
