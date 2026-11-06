cmake_minimum_required(VERSION 3.22)
project(EQ_AI_Mix_Vocals_Full VERSION 1.0.0)

include(FetchContent)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(PluginAssets
    NAMESPACE PluginAssets
    SOURCES
        Resources/EQ_AI_Mix_Vocals_Full_UI.png
        Resources/Arm_Nongmuang_Studio_Logo.png
)

juce_add_plugin(EQ_AI_Mix_Vocals_Full
    COMPANY_NAME "Arm Studio"
    BUNDLE_ID "com.armstudio.eqaimixvocalsfull"
    PLUGIN_MANUFACTURER_CODE ArmS
    PLUGIN_CODE EAVF
    FORMATS VST3
    PRODUCT_NAME "EQ AI Mix Vocals Full"
    DESCRIPTION "AI-assisted vocal EQ and dynamics processor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Generate JuceHeader.h for the source files that include <JuceHeader.h>.
juce_generate_juce_header(EQ_AI_Mix_Vocals_Full)

target_sources(EQ_AI_Mix_Vocals_Full PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)
target_compile_features(EQ_AI_Mix_Vocals_Full PRIVATE cxx_std_17)
target_compile_definitions(EQ_AI_Mix_Vocals_Full PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(EQ_AI_Mix_Vocals_Full PRIVATE
    PluginAssets
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
