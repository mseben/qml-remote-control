cmake_minimum_required(VERSION 3.14)

project(qmlremotecontrol)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Qml Quick WebSockets Widgets Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Qml Quick WebSockets Widgets Concurrent REQUIRED)

add_library(qmlremotecontrol SHARED
    libinit.cpp
    qmlremotecontrol.h
    qmlremotecontrol.cpp
    guicontrol.cpp
    guicontrol.h
)

target_link_libraries(qmlremotecontrol PUBLIC
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick)

target_compile_options(qmlremotecontrol PRIVATE
  -Wall -Wextra -Wpedantic -Werror -Wno-gnu-zero-variadic-macro-arguments -Wno-error=unused-result -Wno-error=deprecated-declarations)

add_subdirectory (qmltestapp)
