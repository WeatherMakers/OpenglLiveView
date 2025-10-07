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

# ===========  第三方库 OpenCV-Mobile  ===========
# 1. 根据鸿蒙构建时的架构变量 (OHOS_ARCH) 设置OpenCV的路径
if(OHOS_ARCH STREQUAL "arm64-v8a")
    set(OpenCV_ARCH_DIR "arm64-v8a")
elseif(OHOS_ARCH STREQUAL "x86_64")
    set(OpenCV_ARCH_DIR "x86_64")
else()
    message(WARNING "OpenCV-Mobile does not have a prebuilt library for the current architecture: ${OHOS_ARCH}. Skipping OpenCV configuration.")
endif()

# 2. 如果找到了对应的架构目录，则继续配置
if(DEFINED OpenCV_ARCH_DIR)
    # 设置 OpenCV_DIR 变量，这是给 find_package 的一个提示，告诉它去哪里找配置文件
    # 这个路径需要指向包含 OpenCVConfig.cmake 文件的目录
    set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7-harmonyos/${OpenCV_ARCH_DIR}/share/OpenCV)

    # 3. 查找OpenCV包。
    #    REQUIRED 表示如果找不到OpenCV，构建过程将报错停止。
    #    NO_MODULE 强制使用 Config 模式，这正是我们所需要的。
    find_package(OpenCV REQUIRED NO_MODULE)

    if(OpenCV_FOUND)
        message(STATUS "Found OpenCV: ${OpenCV_VERSION} in ${OpenCV_DIR}")
        # 4. 将OpenCV的头文件目录添加到项目中
        #    find_package成功后，会自动创建 OpenCV_INCLUDE_DIRS 变量
        include_directories(${OpenCV_INCLUDE_DIRS})
    endif()
endif()
# ===========  第三方库 OpenCV-Mobile 配置结束  ===========

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