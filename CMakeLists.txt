# CMakeLists.txt for Automated Playlist Generation Player

# ==================================================================================================
# Helpers
function(apply_additional_compile_options _target)
    message("-- Adding additional compile options")

    FIND_PACKAGE(ZLIB)
    FIND_PACKAGE(Iconv)

    message("    -- Libiconv found: ${Iconv_FOUND}")
    message("    -- zlib found:     ${ZLIB_FOUND}")
    message("")

    target_link_libraries(${_target} PRIVATE Iconv::Iconv
                                             ZLIB::ZLIB)

    # Ensure Xcode doesn't randomly attempt to codesign
    if (APPLE)
        set_target_properties( APG PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "" )
    endif()

    if(UNIX AND NOT APPLE)
        FIND_PACKAGE(Threads)
        target_link_libraries(${_target} PRIVATE Threads::Threads)
        target_compile_definitions(${_target} PRIVATE JUCE_USE_MP3AUDIOFORMAT=1)
    endif()
endfunction()
#---------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.5" CACHE STRING "Minimum OS X deployment version")

project(APG VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# TODO: Make toggleable
# set(CMAKE_BUILD_TYPE Debug)

set(ESSENTIA_DIR             "${CMAKE_CURRENT_LIST_DIR}/external/essentia")
set(ESSENENTIA_3RD_PARTY_DIR "${ESSENTIA_DIR}/packaging/debian_3rdparty")

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
 if(NOT EXISTS ${JUCE_DIR})
     message(FATAL_ERROR "Missing JUCE path. Set using JUCE_DIR variable")
 endif()
add_subdirectory("${JUCE_DIR}")

# juce_set_vst2_sdk_path(...)

# `juce_add_gui_app` adds an executable target with the name passed as the first argument
# (APG here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_gui_app(APG
    # VERSION ...                       # Set this if the app version is different to the project version
    ICON_BIG resources/waveform.png     # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    PRODUCT_NAME "Automated Playlist Generation Player")     # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# juce_generate_juce_header(APG)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(APG
    PRIVATE
        source/AudioPlayer.cpp
        source/AudioPlayer.h
        source/AudioThumbnailComp.cpp
        source/AudioThumbnailComp.h
        source/BpmDetector.h
        source/Fonts.h
        source/FontsPoppins.cpp
        source/FontsPoppins.h
        source/Main.cpp
        source/MainComponent.cpp
        source/MainComponent.h
        source/MIR.cpp
        source/MIR.h
        source/NonModalAlertWindow.cpp
        source/NonModalAlertWindow.h
        source/PlaylistCreationComponent.cpp
        source/PlaylistCreationComponent.h
        source/PlaylistSettingsComponent.h
        source/PlaylistTrackManagerComponent.cpp
        source/PlaylistTrackManagerComponent.h
        source/SidePanelWithBrowser.h
        source/TransportSlider.h
        source/ThreadWithNonModalAlertWindow.cpp
        source/ThreadWithNonModalAlertWindow.h
        source/WaveformView.cpp
        source/WaveformView.h
      )

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(APG
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:APG,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:APG,JUCE_VERSION>")

if ("${BUILD_TOUCH_UI}" STREQUAL "yes" )
  target_compile_definitions(APG PRIVATE APG_TOUCH_UI)
endif()
# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

juce_add_binary_data(BinaryData
      SOURCES
          resources/cross.png
          resources/play.png
          resources/pause.png
          resources/settings.png
          resources/skip_forward.png
          resources/skip_backward.png
          resources/waveform.png)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_gui_extra` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_include_directories( APG
                            PRIVATE
                            "${ESSENENTIA_3RD_PARTY_DIR}/include/"
                            "${ESSENENTIA_3RD_PARTY_DIR}/include/eigen3"
                            "${ESSENENTIA_3RD_PARTY_DIR}/include/lame/"
                            "${ESSENENTIA_3RD_PARTY_DIR}/include/taglib/"
                            "${ESSENTIA_DIR}/src/essentia/")

target_link_directories( APG
                         PRIVATE
                         "${ESSENENTIA_3RD_PARTY_DIR}/lib"
                         "${ESSENTIA_DIR}/build/src/")

target_link_libraries(APG
    PRIVATE
        BinaryData            # If we'd created a binary data target, we'd link to it here
        juce::juce_gui_extra
        juce::juce_audio_utils
        essentia
        yaml
        avformat
        avcodec
        swresample
        samplerate
        avutil
        chromaprint
        fftw3f
        mp3lame
        tag
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

apply_additional_compile_options(APG)
