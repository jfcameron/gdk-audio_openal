# © Joseph Cameron - MIT License

if (NOT GDKAUDIO_OPENAL_VENDORED_OPENAL)
    find_package(OpenAL CONFIG QUIET)
endif()

if (TARGET OpenAL::OpenAL)
    set_target_properties(OpenAL::OpenAL PROPERTIES IMPORTED_GLOBAL TRUE)

    message(STATUS "gdk-audio_openal: OpenAL from the system (${OpenAL_DIR}); GDKAUDIO_OPENAL_VENDORED_OPENAL builds the vendored one")

    return()
endif()

message(STATUS "gdk-audio_openal: OpenAL Soft built from the vendored source, linked statically")

set(LIBTYPE STATIC)

set(ALSOFT_BACKEND_SNDIO OFF CACHE BOOL "" FORCE)
set(ALSOFT_REQUIRE_SNDIO OFF CACHE BOOL "" FORCE)
set(HAVE_WFUNCTION_EFFECTS 0 CACHE INTERNAL "")
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_ENABLE_MODULES OFF CACHE BOOL "" FORCE)

if (ANDROID)
    set(ALSOFT_DLOPEN OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_PIPEWIRE OFF CACHE BOOL "" FORCE)
    set(ALSOFT_RTKIT OFF CACHE BOOL "" FORCE)
endif()
set(ALSOFT_INSTALL ${GDKAUDIO_OPENAL_INSTALL} CACHE BOOL "" FORCE)

if (MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /WX-")
    string(APPEND CMAKE_C_FLAGS " /WX-")
else()
    string(APPEND CMAKE_CXX_FLAGS " -include cstdlib -Wno-error")
    string(APPEND CMAKE_C_FLAGS " -Wno-error")
endif()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/OpenAL")

