# © Joseph Cameron - All Rights Reserved

get_target_property(_gdk_audio_dir gdkaudio SOURCE_DIR)

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/emitter_group_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/emitter_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/insert_effect_test.cpp"
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

    LIBRARIES
        gdkaudio_openal
        gdkaudio_vorbis
        gdkaudio_conformance)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

set_tests_properties(${JFC_TEST_TARGET} PROPERTIES ENVIRONMENT "ALSOFT_DRIVERS=null")
