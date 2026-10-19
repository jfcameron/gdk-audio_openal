# © Joseph Cameron - All Rights Reserved

set(_jfc_outer_build_tests "${JFC_BUILD_TESTS}")
set(_jfc_outer_build_docs "${JFC_BUILD_DOCS}")
set(_jfc_outer_build_demo "${JFC_BUILD_DEMO}")

set(JFC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO OFF CACHE BOOL "" FORCE)

if (NOT TARGET gdkaudio)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-audio")
endif()

if (NOT TARGET OpenAL)
    set(ALSOFT_BACKEND_SNDIO OFF CACHE BOOL "" FORCE)
    set(ALSOFT_REQUIRE_SNDIO OFF CACHE BOOL "" FORCE)

    set(HAVE_WFUNCTION_EFFECTS 0 CACHE INTERNAL "")

    set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)

    include("${CMAKE_CURRENT_LIST_DIR}/OpenAL.cmake")
endif()

set(JFC_BUILD_TESTS "${_jfc_outer_build_tests}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS "${_jfc_outer_build_docs}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO "${_jfc_outer_build_demo}" CACHE BOOL "" FORCE)
