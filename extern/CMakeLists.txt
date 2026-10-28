# © Joseph Cameron - MIT License

if (NOT TARGET jfc-smart_handles)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-smart_handles")
endif()

if (NOT TARGET gdkaudio)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-audio")
endif()

if (NOT TARGET OpenAL::OpenAL AND NOT EMSCRIPTEN)
    include("${CMAKE_CURRENT_LIST_DIR}/OpenAL.cmake")
endif()
