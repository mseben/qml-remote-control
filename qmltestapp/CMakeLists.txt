project(qmltestapp LANGUAGES CXX)

qt6_add_resources(QT_RESOURCES qml.qrc)

add_executable(qmltestapp main.cpp ${QT_RESOURCES})

#target_include_directories(qmltestapp PRIVATE "${PROJECT_SOURCE_DIR}")
target_include_directories(qmltestapp PRIVATE "../")

target_link_libraries(qmltestapp PUBLIC
    qmlremotecontrol
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick)

target_compile_options(qmltestapp PRIVATE -Wall -Wextra -Wpedantic -Werror)
