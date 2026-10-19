# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

project(gdkaudio_openal
    VERSION 0.0
    DESCRIPTION "game development kit audio, implemented over OpenAL")

option(JFC_BUILD_DEMO "Build the demos" ON)
option(JFC_BUILD_DOCS "Build documentation" OFF)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

add_subdirectory(extern)

file(GLOB_RECURSE _gdk_audio_openal_own_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gdk/audio/*.h")

foreach(_source ${_gdk_audio_openal_own_sources})
    file(READ "${_source}" _contents)

    if (_contents MATCHES "#include[ \t]*<gdk/audio\\.h>")
        message(FATAL_ERROR
            "${_source} includes the umbrella <gdk/audio.h>. It is for consumers; include the "
            "individual headers this file actually needs.")
    endif()
endforeach()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

add_library(gdkaudio_openal STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_efx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_emitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_emitter_group.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_listener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_microphone.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_simple_emitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/openal_stream_emitter.cpp)

add_library(gdk::audio_openal ALIAS gdkaudio_openal)

set_target_properties(gdkaudio_openal PROPERTIES EXPORT_NAME audio_openal)

set_target_properties(gdkaudio_openal PROPERTIES PREFIX "lib")

jfc_generate_build_info(INCLUDE_PATH "gdk/audio_openal")

target_include_directories(gdkaudio_openal
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        "${PROJECT_BINARY_DIR}/generated_include/"
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    )

target_compile_features(gdkaudio_openal PUBLIC cxx_std_20)

target_link_libraries(gdkaudio_openal PUBLIC
    gdkaudio
    jfc-smart_handles
    OpenAL)

if (JFC_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()
