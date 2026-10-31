# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkaudio_openal)
    return()
endif()

project(gdk-audio_openal
    VERSION 1.0
    DESCRIPTION "the gdk audio interface, implemented over OpenAL"
    LANGUAGES C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKAUDIO_OPENAL_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKAUDIO_OPENAL_BUILD_DEMO "Build the demos" OFF)
option(GDKAUDIO_OPENAL_BUILD_DOCS "Build documentation" OFF)
option(GDKAUDIO_OPENAL_INSTALL "Generate install rules, a vendored OpenAL Soft's included" ${PROJECT_IS_TOP_LEVEL})
option(GDKAUDIO_OPENAL_VENDORED_OPENAL "Build OpenAL Soft from the vendored source even where the system has OpenAL" OFF)

if (GDKAUDIO_OPENAL_BUILD_DOCS)
    jfc_generate_documentation_doxygen(
        README_PATH      README.md
        OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs

        INCLUDE_DIRECTORIES
            include/
    )

    if (NOT GDKAUDIO_OPENAL_BUILD_TESTS AND NOT GDKAUDIO_OPENAL_BUILD_DEMO)
        return()
    endif()
endif()

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

add_subdirectory(extern)

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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(GDKAUDIO_OPENAL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_efx.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_emitter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_emitter_group.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_listener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_microphone.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_scene.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_simple_emitter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openal_stream_emitter.cpp")

add_library(gdkaudio_openal STATIC ${GDKAUDIO_OPENAL_SOURCES})

add_library(gdk::audio_openal ALIAS gdkaudio_openal)

set_target_properties(gdkaudio_openal PROPERTIES EXPORT_NAME audio_openal PREFIX "lib")

target_include_directories(gdkaudio_openal
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_compile_features(gdkaudio_openal PUBLIC cxx_std_20)

target_link_libraries(gdkaudio_openal
    PUBLIC
        gdkaudio
    PRIVATE
        $<BUILD_INTERFACE:gdkaudio_implementation>
        $<BUILD_INTERFACE:jfc-smart_handles>)

if (EMSCRIPTEN)
    target_link_options(gdkaudio_openal PUBLIC "-lopenal")
else()
    target_link_libraries(gdkaudio_openal PRIVATE OpenAL::OpenAL)
endif()

if (GDKAUDIO_OPENAL_INSTALL)
    install(TARGETS gdkaudio_openal
        EXPORT gdk-audio_openal-targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(EXPORT gdk-audio_openal-targets
        FILE gdk-audio_openal-targets.cmake
        NAMESPACE gdk::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-audio_openal")

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/gdk-audio_openal-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    set(_gdkaudio_openal_find_openal "find_dependency(Threads)\nfind_dependency(OpenAL CONFIG)\n")

    if (EMSCRIPTEN)
        set(_gdkaudio_openal_find_openal "")
    endif()

    file(WRITE "${PROJECT_BINARY_DIR}/gdk-audio_openal-config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(gdk-audio 1)
${_gdkaudio_openal_find_openal}include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-audio_openal-targets.cmake\")
")

    install(FILES
        "${PROJECT_BINARY_DIR}/gdk-audio_openal-config.cmake"
        "${PROJECT_BINARY_DIR}/gdk-audio_openal-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-audio_openal")
endif()

if (GDKAUDIO_OPENAL_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (GDKAUDIO_OPENAL_BUILD_DEMO)
    add_subdirectory(demo)
endif()
