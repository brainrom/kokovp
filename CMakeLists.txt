cmake_minimum_required(VERSION 3.5)

project(KokoVP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools OpenGLWidgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(mpv REQUIRED mpv)

set(TS_FILES KokoVP_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp
        kokovp.cpp
        kokovp.h

        mpvwidget.cpp
        mpvwidget.h
        playerwidget.h
        playerwidget.cpp
        playercontroller.h
        playercontroller.cpp

        tracksmenu.cpp
        tracksmenu.h

        autohidewidget.cpp
        autohidewidget.h

        config.cpp
        helper.cpp
        cache.cpp
        extensions.cpp

        playlist/playlistmodel.cpp
        playlist/playlistmodel.h

        playlist/playlist.cpp
        playlist/playlist.h

        actions/timelabelaction.cpp
        actions/timelabelaction.h

        actions/volumeslideraction.cpp
        actions/volumeslideraction.h

        actions/timeslider.cpp
        actions/timeslider.h

        actions/timeslideraction.cpp
        actions/timeslideraction.h

        actions/actionwrapper.cpp
        actions/actionwrapper.h

        actions/seekinterface.cpp
        actions/seekinterface.h

        actions/bistableaction.cpp
        actions/bistableaction.h

        actions/incdecactions.cpp
        actions/incdecactions.h


        icons/res.qrc

        prefs/prefdialog.cpp
        prefs/prefdialog.h

        prefs/prefsection.h

        prefs/prefmain.ui
        prefs/prefmain.cpp
        prefs/prefmain.h

        prefs/prefinput.ui
        prefs/prefinput.cpp
        prefs/prefinput.h
        prefs/qactionlistmodel.cpp
        prefs/qactionlistmodel.h
        prefs/shortcuteditordelegate.cpp
        prefs/shortcuteditordelegate.h

        prefs/prefscreenshots.ui
        prefs/prefscreenshots.cpp
        prefs/prefscreenshots.h

        prefs/prefextfile.ui
        prefs/prefextfile.cpp
        prefs/prefextfile.h

        prefs/seekwidget.cpp
        prefs/seekwidget.h

        persistency/filehash.cpp
        persistency/filesettingshash.cpp
        persistency/filesettingshash.h
)

qt_create_translation(QM_FILES ${TS_FILES} ${PROJECT_SOURCES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(KokoVP
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET KokoVP APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    #qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(KokoVP SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(KokoVP
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(KokoVP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGLWidgets ${mpv_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER ru.koko.KokoVP)
endif()
set_target_properties(KokoVP PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include_directories(${CMAKE_SOURCE_DIR} smplayer_imported/)

include(GNUInstallDirs)
install(TARGETS KokoVP
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(KokoVP)
endif()
