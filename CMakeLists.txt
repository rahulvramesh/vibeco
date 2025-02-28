cmake_minimum_required(VERSION 3.30)
project(vibeco VERSION 0.1.0)

# Support Files
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt6") # macOS example

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/include
)

# Add the cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Qt6 COMPONENTS
    Core
    Quick
    Widgets
    Network
    REQUIRED
)

# Add this before find_package(PortAudio REQUIRED)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/opt/homebrew/")

# Find PortAudio using our custom module
find_package(PortAudio REQUIRED)
if(NOT PORTAUDIO_FOUND)
    message(FATAL_ERROR "PortAudio not found. Please install PortAudio development files.")
endif()

# Define source files (only once!)
set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/systemtrayhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/ShortcutManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/macOSShortcutManager.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/macOSShortcutChecker.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/audiohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/audiohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/transcriptionservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/transcriptionservice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/settingsdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/dictationwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/systemtrayhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/ShortcutManager.h
)

# Define resources
set(PROJECT_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/qml/qml.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/icons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/fonts.qrc
)

# Add fonts directory
add_subdirectory(fonts)

# Add QHotkey
add_subdirectory(external/QHotkey)

# Make sure source files are only listed once
set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/systemtrayhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/ShortcutManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/macOSShortcutManager.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/macOSShortcutChecker.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/audiohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/transcriptionservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/settingsdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/dictationwidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/QmlDictationManager.cpp
)

set(PROJECT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/systemtrayhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/ShortcutManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/audiohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/src/transcriptionservice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/settingsdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/dictationwidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include/QmlDictationManager.h
)

add_executable(vibeco
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_RESOURCES}
)

target_link_libraries(vibeco PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Widgets
    Qt6::Network
    QHotkey::QHotkey
    PortAudio::PortAudio
)

if(APPLE)
    set_target_properties(vibeco PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.vibeco.app"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()
