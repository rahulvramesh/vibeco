# FindPortAudio.cmake
# Try to find PortAudio
# Once done, this will define:
#
#  PORTAUDIO_FOUND - system has PortAudio
#  PORTAUDIO_INCLUDE_DIRS - the PortAudio include directories
#  PORTAUDIO_LIBRARIES - link these to use PortAudio

if(APPLE)
    # Try to find Homebrew's PortAudio
    find_path(PORTAUDIO_INCLUDE_DIR
        NAMES portaudio.h
        PATHS
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    )

    find_library(PORTAUDIO_LIBRARY
        NAMES portaudio
        PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    )
else()
    find_path(PORTAUDIO_INCLUDE_DIR
        NAMES portaudio.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
    )

    find_library(PORTAUDIO_LIBRARY
        NAMES portaudio
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
    )
endif()

# Handle the QUIETLY and REQUIRED arguments and set PORTAUDIO_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortAudio DEFAULT_MSG
    PORTAUDIO_LIBRARY PORTAUDIO_INCLUDE_DIR)

if(PORTAUDIO_FOUND)
    set(PORTAUDIO_LIBRARIES ${PORTAUDIO_LIBRARY})
    set(PORTAUDIO_INCLUDE_DIRS ${PORTAUDIO_INCLUDE_DIR})
    
    if(NOT TARGET PortAudio::PortAudio)
        add_library(PortAudio::PortAudio UNKNOWN IMPORTED)
        set_target_properties(PortAudio::PortAudio PROPERTIES
            IMPORTED_LOCATION "${PORTAUDIO_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${PORTAUDIO_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(PORTAUDIO_INCLUDE_DIR PORTAUDIO_LIBRARY) 