cmake_minimum_required(VERSION 3.5)
project(KokoVP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    message(STATUS "CMAKE_BUILD_TYPE is not Debug, setting QT_NO_DEBUG_OUTPUT.")
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools OpenGLWidgets Network Xml)
find_package(PkgConfig REQUIRED)
pkg_check_modules(mpv REQUIRED mpv)

# Libraries
set(LIBRARIES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Xml
    ${mpv_LIBRARIES}
)

set(TS_FILES KokoVP_ru_RU.ts)

qt_wrap_cpp(MOC_FILES
    kokovp.h
    mpvwidget.h
    playerwidget.h
    playercontroller.h
    tracksmenu.h
    autohidewidget.h # From SMPlayer
    singleinstance.h

    playlist/playlistmodel.h
    playlist/playlist.h

    actions/timelabelaction.h # From SMPlayer
    actions/volumeslideraction.h # From SMPlayer
    actions/timeslider.h # From SMPlayer
    actions/timeslideraction.h # From SMPlayer
    actions/actionwrapper.h
    actions/seekinterface.h
    actions/bistableaction.h
    actions/incdecwheelaction.h

    prefs/prefappearance.h
    prefs/prefdialog.h
    prefs/prefsection.h
    prefs/prefmain.h
    prefs/prefinput.h
    prefs/prefscreenshots.h
    prefs/prefextfile.h
    prefs/prefadvanced.h
    prefs/seekwidget.h

    optionsmodel.h
    prefs/qactionlistmodel.h
    prefs/shortcuteditordelegate.h

    persistency/filesettingshash.h # From SMPlayer
)

set(MAIN_SRC
    main.cpp
    kokovp.cpp
    mpvwidget.cpp
    playerwidget.cpp
    playercontroller.cpp
    tracksmenu.cpp
    autohidewidget.cpp # From SMPlayer
    config.cpp
    helper.cpp # From SMPlayer
    cache.cpp
    extensions.cpp # From SMPlayer
    singleinstance.cpp

    optionsmodel.cpp

    playlist/playlistmodel.cpp
    playlist/playlist.cpp
    playlist/m3ucodec.cpp
    playlist/plscodec.cpp
    playlist/xspfcodec.cpp

    actions/timelabelaction.cpp # From SMPlayer
    actions/volumeslideraction.cpp # From SMPlayer
    actions/timeslider.cpp # From SMPlayer
    actions/timeslideraction.cpp
    actions/actionwrapper.cpp
    actions/seekinterface.cpp
    actions/bistableaction.cpp
    actions/incdecwheelaction.cpp

    prefs/prefappearance.cpp
    prefs/prefdialog.cpp
    prefs/prefmain.cpp
    prefs/prefinput.cpp
    prefs/qactionlistmodel.cpp
    prefs/shortcuteditordelegate.cpp
    prefs/prefscreenshots.cpp
    prefs/prefextfile.cpp
    prefs/prefadvanced.cpp
    prefs/seekwidget.cpp

    persistency/filehash.cpp # From SMPlayer
    persistency/filesettingshash.cpp # From SMPlayer
)

set(UI
    prefs/prefappearance.ui
    prefs/prefmain.ui
    prefs/prefinput.ui
    prefs/prefscreenshots.ui
    prefs/prefextfile.ui
    prefs/prefadvanced.ui
)

qt_wrap_ui(FORMS ${UI})
qt_create_translation(QM_FILES ${TS_FILES} ${MAIN_SRC})
qt_add_resources(QRC_CPP icons.qrc)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

add_executable(kokovp ${MOC_FILES} ${FORMS} ${MAIN_SRC} ${QRC_CPP})
target_link_libraries(kokovp ${LIBRARIES})

qt_add_resources(kokovp "translations" BASE ${CMAKE_BINARY_DIR} PREFIX "i18n" FILES ${QM_FILES})
# Awesome ¯\_(ツ)_/¯

include(GNUInstallDirs)
install(TARGETS kokovp
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
