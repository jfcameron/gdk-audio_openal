# © Joseph Cameron - MIT License

get_target_property(_gdk_audio_dir gdkaudio SOURCE_DIR)

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/emitter_group_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/emitter_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/insert_effect_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/lifetime_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/looping_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/microphone_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/multichannel_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/openal_conformance.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/pause_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/playback_position_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/scene_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/send_effect_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vorbis_test.cpp"

    INCLUDE_DIRECTORIES
        "${_gdk_audio_dir}/test"
        "${_gdk_audio_dir}/test/conformance"
        "${PROJECT_SOURCE_DIR}/src/include"

    LIBRARIES
        gdkaudio_openal
        gdkaudio_vorbis
        gdkaudio_conformance
        gdkaudio_implementation
        jfc-smart_handles
        $<IF:$<BOOL:${EMSCRIPTEN}>,,OpenAL::OpenAL>)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

set_tests_properties(${JFC_TEST_TARGET} PROPERTIES ENVIRONMENT "ALSOFT_DRIVERS=null")

if (NOT CMAKE_CROSSCOMPILING AND GDKAUDIO_OPENAL_INSTALL)
    add_test(NAME gdk-audio_openal_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")

    set_tests_properties(gdk-audio_openal_install_and_consume PROPERTIES ENVIRONMENT "ALSOFT_DRIVERS=null")
endif()
