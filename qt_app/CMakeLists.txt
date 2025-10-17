cmake_minimum_required(VERSION 3.16)

project(Debug_ToolV2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Debug_ToolV2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        actionset.h actionset.cpp
        action.h

        handlers/main/addAction.cpp

        actionwindow.h actionwindow.cpp actionwindow.ui
        utils/main/rebuildList.cpp
        utils/main/actionInfoLine.cpp



        utils/main/refreshPorts.cpp
        handlers/main/connect.cpp
        serialmonitor.h serialmonitor.cpp serialmonitor.ui
        handlers/main/serialMonitor.cpp
        actionEncoder.h
        utils/main/actionEncoder.cpp
        handlers/main/execute.cpp
        handlers/main/deleteAction.cpp
        handlers/action/add.cpp
        handlers/action/cancel.cpp
        utils/action/hL.cpp
        utils/action/parseIds.cpp

        handlers/main/editAction.cpp
        utils/action/setEditMode.cpp
        utils/action/setKindPage.cpp
        utils/action/currentKind.cpp
        utils/action/level.cpp
        utils/action/csv.cpp
        utils/action/fillFromAction.cpp
        utils/action/makeAction.cpp
        utils/action/log/logTx.cpp
        utils/action/log/coloredLog.cpp
        utils/main/sendInstruction.cpp
        handlers/main/writeFlash.cpp
        handlers/main/clearFlash.cpp
        handlers/main/reset.cpp
        utils/action/parseTime.cpp
        cspiwindow.h cspiwindow.cpp cspiwindow.ui
        handlers/main/cSPI.cpp
        handlers/cspi/set.cpp
        handlers/cspi/clr.cpp
        handlers/cspi/import.cpp
        handlers/cspi/stop.cpp
        utils/main/onSPIStopRequested.cpp
        handlers/cspi/terminate.cpp





    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Debug_ToolV2 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Debug_ToolV2 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Debug_ToolV2
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Debug_ToolV2 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Debug_ToolV2)
endif()
set_target_properties(Debug_ToolV2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Debug_ToolV2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Debug_ToolV2)
endif()
