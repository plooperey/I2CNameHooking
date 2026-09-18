cmake_minimum_required(VERSION 3.22.1)

project(relictatest)

set(KITTYMEMORY_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/Include/KittyMemory"
)

file(GLOB KITTYMEMORY_SRC
        "${KITTYMEMORY_PATH}/*.cpp"
)

set(KEYSTONE_LIB
        "${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a"
)

set(BUILD_SHARED_LIBS OFF)
set(DOBBY_GENERATE_SHARED OFF CACHE BOOL "" FORCE)

add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby"
        "${CMAKE_CURRENT_BINARY_DIR}/dobby"
)

add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp
        UnityString/UnityString.cpp
        Class/Class.cpp
        Utils/Utils.cpp
        Method/Method.cpp
        Method/InnerMethod.cpp
        Il2CppMethods/Il2CppMethods.cpp
        ${KITTYMEMORY_SRC}
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby/source
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby/source/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby/external
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby/external/logging
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Dobby/external/logging/logging
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${KEYSTONE_LIB}
        dobby
        android
        log
)