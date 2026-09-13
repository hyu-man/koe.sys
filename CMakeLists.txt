cmake_minimum_required(VERSION 3.22)

project(KOE_SYS VERSION 0.1.0)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../JUCE JUCE)

juce_add_plugin(KoeSys
    COMPANY_NAME "vicugna"
    PLUGIN_MANUFACTURER_CODE Vcgn
    PLUGIN_CODE Koe1
    FORMATS VST3 Standalone
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Assets/icon.png"
    PRODUCT_NAME "koe.sys"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_add_binary_data(KoeSysData SOURCES
    Assets/DotGothic16-Regular.ttf)

target_sources(KoeSys PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_include_directories(KoeSys PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/signalsmith-stretch-main)

target_compile_options(KoeSys PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

target_compile_definitions(KoeSys PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(KoeSys
    PRIVATE
        KoeSysData
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
