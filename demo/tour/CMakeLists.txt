# © Joseph Cameron - MIT License

project(gdkaudio-demo)

add_executable(gdkaudio-demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(gdkaudio-demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(gdkaudio-demo PRIVATE gdkaudio_openal gdkaudio_vorbis)

target_compile_features(gdkaudio-demo PRIVATE cxx_std_20)

