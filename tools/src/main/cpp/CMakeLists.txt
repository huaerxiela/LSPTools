# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("tools")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -std=c++17 -Wl,-Bsymbolic")
#添加内联支持
enable_language(C ASM)



include_directories(
        ./
        ./log/
        ./dobby/
        ./uitils/
        ./pending/

)

find_library(
        log-lib

        log
)

add_library(
        tracetest

        SHARED

        test/test.cpp
)



add_library(
        jnitrace

        SHARED

        jnitrace/main.cpp
        jnitrace/JnitraceForC.cpp
        uitils/parse.cpp
)
add_library(
        pending

        SHARED

        pending/pending_hook.cpp
        pending/hide_api.cpp
        pending/dlfcn_nougat.cpp
)

# 使用预编译库
add_library(local_dobby STATIC IMPORTED)
set_target_properties(local_dobby PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${ANDROID_ABI}/libdobby.a)
macro(SET_OPTION option value)
    set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()
SET_OPTION(DOBBY_DEBUG OFF)

target_link_libraries(
        jnitrace

        local_dobby
        ${log-lib}
)
target_link_libraries(
        pending

        local_dobby
        ${log-lib}
)


target_link_libraries(
        tracetest

        ${log-lib}
)


